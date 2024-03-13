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
#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <tuple>
#include <utility>

namespace cppinyin {

void PinyinEncoder::Build(const std::string &vocab_path) {
  LoadVocab(vocab_path);

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
}

void PinyinEncoder::GetDag(const std::string &str, DagType *dag) const {
  dag->resize(str.size());
  for (int32_t i = 0; i < str.size(); ++i) {
    int32_t MAX_HIT = str.size() - i;
    const char *query = str.data() + i;
    std::vector<int32_t> results(MAX_HIT);
    std::size_t num_matches =
        da_.commonPrefixSearch(query, results.data(), MAX_HIT);
    std::vector<DagItem> items;
    for (int32_t j = 0; j < num_matches; ++j) {
      int32_t idx = results[j];
      std::string tmp = tokens_[idx];
      items.push_back(std::make_tuple(scores_[idx], i + tmp.size(), idx));
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
                        std::vector<std::string> *ostrs) const {
  ostrs->clear();
  int32_t i = 0;
  int32_t fail_bytes = 0;
  while (i < str.size()) {
    int32_t next_index = std::get<1>(route[i]);
    if (next_index == -1) {
      fail_bytes += 1;
      i += 1;
    } else {
      if (fail_bytes != 0) {
        ostrs->push_back(str.substr(i - fail_bytes, fail_bytes));
      }
      fail_bytes = 0;
      auto &values = values_[std::get<2>(route[i])];
      ostrs->insert(ostrs->end(), values.begin(), values.end());
      i = next_index;
    }
  }
  if (fail_bytes != 0) {
    ostrs->push_back(str.substr(i - fail_bytes, fail_bytes));
  }
}

void PinyinEncoder::EncodeBase(const std::string &str,
                               std::vector<DagItem> *route) const {
  DagType dag;
  GetDag(str, &dag);
  CalcDp(str, dag, route);
}

void PinyinEncoder::EncodeBase(const std::string &str,
                               std::vector<std::string> *ostrs) const {
  std::vector<DagItem> route;
  EncodeBase(str, &route);
  Cut(str, route, ostrs);
}

void PinyinEncoder::Encode(const std::string &str,
                           std::vector<std::string> *ostrs) const {
  ostrs->clear();
  std::vector<std::string> substrs;
  std::string word;
  std::istringstream iss(str);
  while (iss >> word) {
    substrs.push_back(word);
  }
  std::vector<std::vector<std::string>> subostrs(substrs.size());
  std::vector<std::future<void>> results;
  for (int32_t i = 0; i < subostrs.size(); ++i) {
    results.emplace_back(pool_->enqueue([this, i, &substrs, &subostrs] {
      return this->EncodeBase(substrs[i], &(subostrs[i]));
    }));
  }
  for (auto &&result : results) {
    result.get();
  }
  for (int32_t i = 0; i < subostrs.size(); ++i) {
    ostrs->insert(ostrs->end(), subostrs[i].begin(), subostrs[i].end());
  }
}

void PinyinEncoder::Encode(const std::vector<std::string> &strs,
                           std::vector<std::vector<std::string>> *ostrs) const {
  ostrs->resize(strs.size());
  std::vector<std::future<void>> results;
  for (int32_t i = 0; i < strs.size(); ++i) {
    results.emplace_back(pool_->enqueue([this, i, &strs, ostrs] {
      return this->Encode(strs[i], &((*ostrs)[i]));
    }));
  }

  for (auto &&result : results) {
    result.get();
  }
}

void PinyinEncoder::LoadVocab(const std::string &vocab_path) {
  std::ifstream is(vocab_path);
  if (!is) {
    std::cerr << "Open vocab file failed : " << vocab_path.c_str();
    exit(-1);
  }
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
      values.push_back(value);
    }
    if (values.empty()) {
      std::cerr << "Each line in vocab should contain at lease three items "
                   "(seperate by space), "
                   "the first one is Chinese word/character, the second one is "
                   "score, the others are the corresponding pinyins given : "
                << line.c_str();
      exit(-1);
    }
    values_.emplace_back(std::move(values));
  }
}

} // namespace cppinyin
