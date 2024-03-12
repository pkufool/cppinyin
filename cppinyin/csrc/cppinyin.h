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

#ifndef SSENTENCEPIECE_CSRC_SSENTENCEPIECE_H_
#define SSENTENCEPIECE_CSRC_SSENTENCEPIECE_H_

#include "ssentencepiece/csrc/darts.h"
#include "ssentencepiece/csrc/ssentencepiece.h"
#include "ssentencepiece/csrc/threadpool.h"
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace ssentencepiece {

class Ssentencepiece {
  // <token score, index into input str, index into tokens>
  using DagItem = std::tuple<float, int32_t, int32_t>;
  using DagType = std::vector<std::vector<DagItem>>;

public:
  Ssentencepiece(const std::string &vocab_path,
                 int32_t num_threads = std::thread::hardware_concurrency()) {
    pool_ = std::make_unique<ThreadPool>(num_threads);
    Build(vocab_path);
  }
  Ssentencepiece(int32_t num_threads = std::thread::hardware_concurrency()) {
    pool_ = std::make_unique<ThreadPool>(num_threads);
  }
  ~Ssentencepiece() {}

  void Build(const std::string &vocab_path);

  void Encode(const std::string &str, std::vector<std::string> *ostrs) const;

  void Encode(const std::string &str, std::vector<int32_t> *oids) const;

  void Encode(const std::vector<std::string> &strs,
              std::vector<std::vector<std::string>> *ostrs) const;

  void Encode(const std::vector<std::string> &strs,
              std::vector<std::vector<int32_t>> *oids) const;

  std::string Decode(const std::vector<int32_t> &ids) const;

  std::vector<std::string>
  Decode(const std::vector<std::vector<int32_t>> &ids) const;

private:
  void LoadVocab(const std::string &vocab_path);

  std::string Encode(const std::string &str, std::vector<DagItem> *route) const;

  void GetDag(const std::string &str, DagType *dag) const;

  void CalcDp(const std::string &str, const DagType &dag,
              std::vector<DagItem> *route) const;

  void Cut(const std::string &str, const std::vector<DagItem> &route,
           std::vector<std::string> *ostrs) const;

  void Cut(const std::string &str, const std::vector<DagItem> &route,
           std::vector<int32_t> *oids) const;

  bool fallback_bytes_ = false;
  int32_t bytes_offset_ = 0;
  int32_t unk_id_ = 0;
  std::vector<std::string> tokens_;
  std::vector<float> scores_;
  std::unique_ptr<ThreadPool> pool_;
  Darts::DoubleArray da_;
};

} // namespace ssentencepiece

#endif // SSENTENCEPIECE_CSRC_SSENTENCEPIECE_H_
