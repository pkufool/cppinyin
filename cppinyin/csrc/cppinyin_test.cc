/**
 * Copyright      2024  Wei Kang (wkang@pku.edu.cn)
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

#include "gtest/gtest.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cppinyin/csrc/cppinyin.h"

namespace cppinyin {

TEST(PinyinEncoder, TestEncode) {
  std::string vocab_path = "data/pinyin.txt";
  PinyinEncoder processor(vocab_path);

  std::string str = "我是中国 人我爱我的 love you 祖国";
  std::vector<std::string> pieces;
  processor.Encode(str, &pieces);

  std::ostringstream oss;
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  std::cerr << oss.str() << std::endl;
}

} // namespace cppinyin
