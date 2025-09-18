/**
 * Copyright      2024    Wei Kang (wkang@pku.edu.cn)
 *
 * See LICENSE for clarification regarding multiple authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cppinyin/csrc/cppinyin.h"
#include "cppinyin/csrc/utils.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace cppinyin {

void PinyinEncoder::Init(int32_t num_threads) {
  if (num_threads <= 0) {
    num_threads = std::thread::hardware_concurrency();
  }
  pool_ = std::make_unique<ThreadPool>(num_threads);
  tone_to_normal_.reserve(NORMAL_TO_TONE.size());
  for (const auto &item : NORMAL_TO_TONE) {
    tone_to_normal_[item.second] = item.first;
    auto value = RemoveNumberTone(item.second);
    no_tone_set_.insert(value);
  }
}

std::vector<std::string>
PinyinEncoder::AllPinyin(const std::string &tone /*=number*/,
                         bool partial /*=false*/) const {

  std::vector<std::string> pinyins;
  if (partial) {
    auto initials = AllInitials();
    for (const auto &initial : initials) {
      pinyins.push_back(initial);
    }
    auto finals = AllFinals(tone);
    for (const auto &final : finals) {
      pinyins.push_back(final);
    }
    return pinyins;
  }
  if (tone == "none") {
    for (const auto &value : no_tone_set_) {
      pinyins.push_back(value);
    }
  } else if (tone == "normal") {
    for (const auto &item : NORMAL_TO_TONE) {
      pinyins.push_back(item.first);
    }
  } else if (tone == "number") {
    for (const auto &item : NORMAL_TO_TONE) {
      pinyins.push_back(item.second);
    }
  } else {
    std::cerr << "PinyinEncoder: Invalid tone type: " << tone << std::endl;
    std::abort();
  }
  std::sort(pinyins.begin(), pinyins.end());
  return pinyins;
}

std::vector<std::string> PinyinEncoder::AllInitials() const {
  std::set<std::string> initial_set;
  for (const auto &item : NORMAL_TO_TONE) {
    auto initial = GetInitial(item.first);
    if (!initial.empty()) {
      initial_set.insert(initial);
    }
  }
  std::vector<std::string> initials;
  initials.reserve(initial_set.size());
  for (const auto &initial : initial_set) {
    initials.push_back(initial);
  }
  return initials;
}

std::vector<std::string>
PinyinEncoder::AllFinals(const std::string &tone /*=number*/) const {
  auto pinyins = AllPinyin(tone, false);

  std::set<std::string> finals_set;
  for (const auto &value : pinyins) {
    auto initial = GetInitial(value);
    auto final_t = value.substr(initial.size());
    if (!final_t.empty()) {
      finals_set.insert(final_t);
    }
  }

  std::vector<std::string> finals;
  finals.reserve(finals_set.size());
  for (const auto &value : finals_set) {
    finals.push_back(value);
  }
  return finals;
}

bool PinyinEncoder::ValidPinyin(const std::string &s,
                                const std::string &tone /*=number*/) const {
  if (tone == "none") {
    return no_tone_set_.find(s) != no_tone_set_.end();
  } else if (tone == "normal") {
    return NORMAL_TO_TONE.find(s) != NORMAL_TO_TONE.end();
  } else if (tone == "number") {
    return tone_to_normal_.find(s) != tone_to_normal_.end();
  } else {
    CPY_ASSERT(tone.empty(),
               "tone should be empty of one of 'number', 'none' and 'normal'");
    return no_tone_set_.find(s) != no_tone_set_.end() ||
           NORMAL_TO_TONE.find(s) != NORMAL_TO_TONE.end() ||
           tone_to_normal_.find(s) != tone_to_normal_.end();
  }
}

void PinyinEncoder::Build(std::istream &is) {
  LoadVocab(is);

  std::vector<const char *> keys(tokens_.size());
  std::vector<size_t> length(tokens_.size());
  std::vector<int32_t> values(tokens_.size());

  std::iota(values.begin(), values.end(), 0);

  std::stable_sort(values.begin(), values.end(),
                   [&tokens = tokens_](size_t i1, size_t i2) {
                     return tokens[i1] < tokens[i2];
                   });

  for (int32_t i = 0; i < values.size(); ++i) {
    keys[i] = tokens_[values[i]].c_str();
    length[i] = tokens_[values[i]].size();
  }

  da_.build(keys.size(), keys.data(), length.data(), values.data());
  tokens_.clear();
}

void PinyinEncoder::GetDag(const std::string &str, DagType *dag) const {
  dag->resize(str.size());
  for (int32_t i = 0; i < str.size(); ++i) {
    int32_t MAX_HIT = str.size() - i;
    const char *query = str.data() + i;
    std::vector<Darts::DoubleArray::result_pair_type> results(MAX_HIT);
    std::size_t num_matches =
        da_.commonPrefixSearch(query, results.data(), MAX_HIT);
    std::vector<DagItem> items;
    for (int32_t j = 0; j < num_matches; ++j) {
      int32_t idx = results[j].value;
      int32_t length = results[j].length;
      items.push_back(std::make_tuple(scores_[idx], i + length, idx));
    }
    (*dag)[i] = items;
  }
}

void PinyinEncoder::CalcDp(const std::string &str, const DagType &dag,
                           std::vector<DagItem> *route) const {
  route->resize(str.size() + 1);
  (*route)[str.size()] = std::make_tuple(0.0, 0, 0);
  for (int32_t i = str.size() - 1; i >= 0; i--) {
    float max_score = -std::numeric_limits<float>::infinity();
    int32_t max_idx = -1;
    int32_t index = 0;
    for (const auto &item : dag[i]) {
      float score =
          std::get<0>(item) + std::get<0>((*route)[std::get<1>(item)]);
      if (score > max_score) {
        max_score = score;
        max_idx = std::get<1>(item);
        index = std::get<2>(item);
      } else if (score == max_score) {
        if (max_idx >= std::get<1>(item)) {
          max_idx = std::get<1>(item);
          index = std::get<2>(item);
        }
      } else {
        continue;
      }
    }
    (*route)[i] = std::make_tuple(
        max_score == -std::numeric_limits<float>::infinity() ? 0 : max_score,
        max_idx, index);
  }
}

void PinyinEncoder::Cut(const std::string &str,
                        const std::vector<DagItem> &route,
                        const std::string &tone, bool partial,
                        std::vector<std::string> *ostrs,
                        std::vector<std::string> *segs) const {
  ostrs->clear();
  segs->clear();
  int32_t i = 0;
  int32_t fail_bytes = 0;
  while (i < str.size()) {
    int32_t next_index = std::get<1>(route[i]);
    if (next_index == -1) {
      fail_bytes += 1;
      i += 1;
    } else {
      if (fail_bytes != 0) {
        ostrs->emplace_back(str.substr(i - fail_bytes, fail_bytes));
        segs->emplace_back(str.substr(i - fail_bytes, fail_bytes));
      }
      fail_bytes = 0;
      for (const auto &value : values_[std::get<2>(route[i])]) {
        auto value_t = value;
        auto it = tone_to_normal_.find(value);
        if (it == tone_to_normal_.end()) {
          ostrs->push_back(value_t);
          continue;
        }
        if (tone == "normal") {
          value_t = it->second;
        }
        if (partial) {
          auto initial = GetInitial(value_t);
          auto final_t = value_t.substr(initial.size());
          if (tone == "none") {
            final_t = RemoveTone(final_t);
          }
          if (!initial.empty()) {
            ostrs->push_back(initial);
          }
          ostrs->push_back(final_t);
        } else {
          if (tone == "none") {
            ostrs->push_back(RemoveTone(value_t));
          } else {
            ostrs->push_back(value_t);
          }
        }
      }
      segs->emplace_back(str.substr(i, next_index - i));
      i = next_index;
    }
  }
  if (fail_bytes != 0) {
    ostrs->emplace_back(str.substr(i - fail_bytes, fail_bytes));
    segs->emplace_back(str.substr(i - fail_bytes, fail_bytes));
  }
}

void PinyinEncoder::EncodeBase(const std::string &str,
                               std::vector<DagItem> *route) const {
  DagType dag;
  GetDag(str, &dag);
  CalcDp(str, dag, route);
}

void PinyinEncoder::EncodeBase(const std::string &str,
                               std::vector<std::string> *ostrs,
                               const std::string &tone, bool partial,
                               std::vector<std::string> *segs) const {
  std::vector<DagItem> route;
  EncodeBase(str, &route);
  Cut(str, route, tone, partial, ostrs, segs);
}

void PinyinEncoder::Encode(const std::string &str,
                           std::vector<std::string> *ostrs,
                           const std::string &tone /*=number*/,
                           bool partial /*=false*/,
                           std::vector<std::string> *segs /*=nullptr*/) const {
  CPY_ASSERT(tone == "number" || tone == "none" || tone == "normal",
             "tone should be one of 'number', 'none' and 'normal'");
  ostrs->clear();
  std::vector<std::string> substrs;
  std::string word;
  std::istringstream iss(str);
  while (iss >> word) {
    substrs.push_back(word);
  }
  std::vector<std::vector<std::string>> subostrs(substrs.size());
  std::vector<std::vector<std::string>> subsegs(substrs.size());
  for (int32_t i = 0; i < subostrs.size(); ++i) {
    EncodeBase(substrs[i], &(subostrs[i]), tone, partial, &(subsegs[i]));
  }

  for (int32_t i = 0; i < subostrs.size(); ++i) {
    ostrs->insert(ostrs->end(), subostrs[i].begin(), subostrs[i].end());
  }
  if (segs != nullptr) {
    segs->clear();
    for (int32_t i = 0; i < subsegs.size(); ++i) {
      segs->insert(segs->end(), subsegs[i].begin(), subsegs[i].end());
    }
  }
}

void PinyinEncoder::Encode(
    const std::vector<std::string> &strs,
    std::vector<std::vector<std::string>> *ostrs,
    const std::string &tone /*=number*/, bool partial /*=false*/,
    std::vector<std::vector<std::string>> *segs /*=nullptr*/) const {
  CPY_ASSERT(tone == "number" || tone == "none" || tone == "normal",
             "tone should be one of 'number', 'none' and 'normal'");
  ostrs->resize(strs.size());
  if (segs != nullptr) {
    segs->resize(strs.size());
  }
  std::vector<std::future<void>> results;
  for (int32_t i = 0; i < strs.size(); ++i) {
    results.emplace_back(pool_->enqueue([this, i, &strs, ostrs, tone, partial,
                                         segs] {
      if (segs != nullptr) {
        return this->Encode(strs[i], &((*ostrs)[i]), tone, partial,
                            &((*segs)[i]));
      } else {
        return this->Encode(strs[i], &((*ostrs)[i]), tone, partial, nullptr);
      }
    }));
  }
  for (auto &&result : results) {
    result.get();
  }
}

void PinyinEncoder::LoadVocab(std::istream &is) {
  tokens_.clear();
  scores_.clear();
  values_.clear();
  std::string line;
  std::string token;
  std::string value;
  float score;
  while (std::getline(is, line)) {
    std::istringstream iss(line);
    iss >> token >> score;
    tokens_.push_back(token);
    scores_.push_back(score);
    std::vector<std::string> values;
    while (iss >> value) {
      // Always convert to tone in internal
      auto it = NORMAL_TO_TONE.find(value);
      if (it != NORMAL_TO_TONE.end()) {
        value = it->second;
      }
      values.push_back(value);
    }
    if (values.empty()) {
      std::cerr << "Each line in vocab should contain at lease three items "
                   "(seperate by space), "
                   "the first one is Chinese word/character, the second one is "
                   "score, the others are the corresponding pinyins given : "
                << line.c_str() << std::endl;
      exit(-1);
    }
    values_.emplace_back(std::move(values));
  }
}

std::string PinyinEncoder::GetInitial(const std::string &s) const {
  std::size_t pos = s.find_first_of(INITIALS);
  if (pos == 0) {
    std::size_t npos = s.find_first_not_of(INITIALS, 1);
    if (npos == std::string::npos) {
      return s.substr(0);
    } else {
      // Handle m̄ : 0x6d 0xcc 0x7c and m̀ : 0x6d 0xcc 0x80
      // m is 0x6d
      const uint8_t *p = reinterpret_cast<const uint8_t *>(s.c_str());
      if (p[pos] == 0x6d && p[pos + 1] == 0xcc) {
        return std::string();
      } else {
        return s.substr(0, npos - pos);
      }
    }
  }
  return std::string();
}

std::string PinyinEncoder::RemoveTone(const std::string &s) const {
  return RemoveNumberTone(s);

  std::string phonetic;
  std::size_t len;
  std::size_t pos = s.find_first_of(PHONETICS);

  // Handle m̄ : 0x6d 0xcc 0x7c and m̀ : 0x6d 0xcc 0x80
  // m is 0x6d
  const uint8_t *p = reinterpret_cast<const uint8_t *>(s.c_str());
  if (p[pos] == 0x6d) {
    if ((p[pos + 1] == 0xcc && p[pos + 2] == 0x7c) ||
        (p[pos + 1] == 0xcc && p[pos + 2] == 0x80)) {
      // do nothing
    } else {
      pos = s.find_first_of(PHONETICS, pos + 1);
    }
  }

  // Handle ü : 0xc3 0xbc not a phonetic
  if (p[pos] == 0xc3 && p[pos + 1] == 0xbc) {
    pos = s.find_first_of(PHONETICS, pos + 1);
  }

  if (pos == std::string::npos) {
    return s;
  } else {
    std::size_t npos = s.find_first_not_of(PHONETICS, pos + 1);
    if (npos == std::string::npos) {
      phonetic = s.substr(pos);
      len = s.size() - pos;
    } else {
      len = npos - pos;
      phonetic = s.substr(pos, len);
    }
  }

  std::string phone = PHONETICS_MAP.at(phonetic);

  std::ostringstream oss;
  oss << s.substr(0, pos) << phone << s.substr(pos + len);
  return oss.str();
}

std::string PinyinEncoder::ToInitial(const std::string &s) const {
  if (s.empty()) {
    return s;
  }
  if (tone_to_normal_.count(s) == 0 && NORMAL_TO_TONE.count(s) == 0) {
    // std::cerr << "ToInitial: " << s << " is not a valid pinyin. " <<
    // std::endl;
    return std::string();
  }
  return GetInitial(s);
}

void PinyinEncoder::ToInitials(const std::vector<std::string> &strs,
                               std::vector<std::string> *ostrs) const {
  ostrs->clear();
  for (const auto &s : strs) {
    ostrs->push_back(ToInitial(s));
  }
}

std::string PinyinEncoder::ToFinal(const std::string &s,
                                   const std::string &tone /*=number*/) const {
  CPY_ASSERT(tone == "number" || tone == "none" || tone == "normal",
             "tone should be one of 'number', 'none' and 'normal'");
  if (s.empty()) {
    return s;
  }
  std::string value;
  if (tone_to_normal_.find(s) != tone_to_normal_.end()) {
    value = s;
  } else if (NORMAL_TO_TONE.find(s) != NORMAL_TO_TONE.end()) {
    value = NORMAL_TO_TONE.at(s);
  } else {
    // std::cerr << "ToFinal: " << s << " is not a valid pinyin. " << std::endl;
    return std::string();
  }
  if (tone == "none") {
    value = RemoveTone(value);
  } else if (tone == "normal") {
    value = tone_to_normal_.at(value);
  } else {
    // tone == "number"
    // Do nothing, value is already in number tone
  }
  auto initial = GetInitial(value);
  auto final_t = value.substr(initial.size());
  return final_t;
}

void PinyinEncoder::ToFinals(const std::vector<std::string> &strs,
                             std::vector<std::string> *ostrs,
                             const std::string &tone /*=number*/
) const {
  CPY_ASSERT(tone == "number" || tone == "none" || tone == "normal",
             "tone should be one of 'number', 'none' and 'normal'");
  ostrs->clear();
  for (const auto &s : strs) {
    ostrs->push_back(ToFinal(s, tone));
  }
}

size_t PinyinEncoder::SaveValues(const std::string &model_path) const {
  std::ofstream of(model_path, std::ofstream::binary);
  size_t offset = 0;

  // Write header
  offset += WriteHeader(of);

  // Save scores_
  offset += WriteUint32(of, scores_.size());
  for (const auto score : scores_) {
    offset += WriteFloat(of, score);
  }
  // Save values_
  offset += WriteUint32(of, values_.size());
  for (const auto &value : values_) {
    offset += WriteUint32(of, value.size());
    for (const auto &v : value) {
      offset += WriteString(of, v);
    }
  }
  of.close();
  return offset;
}

size_t PinyinEncoder::LoadValues(std::istream &ifile) {
  size_t offset = 0;
  uint32_t size;

  // Load scores_
  offset += ReadUint32(ifile, &size);
  scores_.resize(size);
  for (uint32_t i = 0; i < size; ++i) {
    offset += ReadFloat(ifile, &(scores_[i]));
  }

  // Load values_
  offset += ReadUint32(ifile, &size);
  values_.resize(size);
  uint32_t sub_size;
  std::string value;
  for (uint32_t i = 0; i < size; ++i) {
    offset += ReadUint32(ifile, &sub_size);
    values_[i].resize(sub_size);
    for (uint32_t j = 0; j < sub_size; ++j) {
      offset += ReadString(ifile, &value);
      // Always convert to number tone in internal
      auto it = NORMAL_TO_TONE.find(value);
      if (it != NORMAL_TO_TONE.end()) {
        value = it->second;
      }
      values_[i][j] = value;
    }
  }
  return offset;
}

void PinyinEncoder::Load(const std::string &model_path) {
  std::ifstream ifile(model_path, std::ifstream::binary);
  Load(ifile);
}

void PinyinEncoder::Load(std::istream &is) {
  std::string value;
  ReadHeader(is, &value);

  if (HEADER != value) {
    is.seekg(0, std::ios::beg);
    return Build(is);
  }

  size_t offset = LoadValues(is) + value.size();
  da_.open(is, offset);
}

void PinyinEncoder::Save(const std::string &model_path) const {
  SaveValues(model_path);
  da_.save(model_path.c_str(), "ab", 0);
}

} // namespace cppinyin
