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

#ifndef CPPINYIN_CSRC_CPPINYIN_H_
#define CPPINYIN_CSRC_CPPINYIN_H_

#include "cppinyin/csrc/cppinyin.h"
#include "cppinyin/csrc/darts.h"
#include "cppinyin/csrc/pinyin.h"
#include "cppinyin/csrc/threadpool.h"
#include "cppinyin/csrc/utils.h"
#include <cstdlib>
#include <fstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace cppinyin {

class PinyinEncoder {
  // <token score, index into input str, index into tokens>
  using DagItem = std::tuple<float, int32_t, int32_t>;
  using DagType = std::vector<std::vector<DagItem>>;

public:
  PinyinEncoder(const std::string &vocab_path,
                int32_t num_threads = std::thread::hardware_concurrency()) {
    Init(num_threads);
    auto is = std::fstream(vocab_path);
    Load(is);
  }

  PinyinEncoder(std::istream &is,
                int32_t num_threads = std::thread::hardware_concurrency()) {
    Init(num_threads);
    Load(is);
  }

  PinyinEncoder(int32_t num_threads = std::thread::hardware_concurrency()) {
    Init(num_threads);
  }

  ~PinyinEncoder() {}

  std::vector<std::string> AllPinyin(const std::string &tone = "number",
                                     bool partial = false) const;

  std::vector<std::string> AllInitials() const;

  std::vector<std::string> AllFinals(const std::string &tone = "number") const;

  bool ValidPinyin(const std::string &s, const std::string &tone = "") const;

  void Encode(const std::string &str, std::vector<std::string> *ostrs,
              const std::string &tone = "number", bool partial = false,
              std::vector<std::string> *segs = nullptr) const;

  void Encode(const std::vector<std::string> &strs,
              std::vector<std::vector<std::string>> *ostrs,
              const std::string &tone = "number", bool partial = false,
              std::vector<std::vector<std::string>> *segs = nullptr) const;

  std::string ToInitial(const std::string &s) const;
  void ToInitials(const std::vector<std::string> &strs,
                  std::vector<std::string> *ostrs) const;

  std::string ToFinal(const std::string &s,
                      const std::string &tone = "number") const;
  void ToFinals(const std::vector<std::string> &strs,
                std::vector<std::string> *ostrs,
                const std::string &tone = "number") const;

  void Load(const std::string &model_path);
  void Load(std::istream &is);

  void Save(const std::string &model_path) const;

private:
  void Init(int32_t num_threads);

  void Build(std::istream &is);

  void LoadVocab(std::istream &is);

  void EncodeBase(const std::string &str, std::vector<DagItem> *route) const;

  void EncodeBase(const std::string &str, std::vector<std::string> *ostrs,
                  const std::string &tone, bool partial,
                  std::vector<std::string> *segs) const;

  void GetDag(const std::string &str, DagType *dag) const;

  void CalcDp(const std::string &str, const DagType &dag,
              std::vector<DagItem> *route) const;

  void Cut(const std::string &str, const std::vector<DagItem> &route,
           const std::string &tone, bool partial,
           std::vector<std::string> *ostrs,
           std::vector<std::string> *segs) const;

  std::string GetInitial(const std::string &s) const;

  std::string RemoveTone(const std::string &s) const;

  size_t SaveValues(const std::string &model_path) const;
  size_t LoadValues(std::istream &ifile);

  std::unordered_map<std::string, std::string> tone_to_normal_;
  std::unordered_set<std::string> no_tone_set_;
  std::vector<std::string> tokens_;
  std::vector<float> scores_;
  std::vector<std::vector<std::string>> values_;
  std::unique_ptr<ThreadPool> pool_;
  Darts::DoubleArray da_;
};

} // namespace cppinyin

#endif // CPPINYIN_CSRC_CPPINYIN_H_
