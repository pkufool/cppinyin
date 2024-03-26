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

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cppinyin/csrc/cppinyin.h"

namespace cppinyin {

TEST(PinyinEncoder, TestEncode) {
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin.raw";
  PinyinEncoder processor(vocab_path);

  std::string str = "我是中国 人我爱我的 love you 祖国";
  std::vector<std::string> pieces;
  processor.Encode(str, &pieces);

  std::ostringstream oss;
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  std::cerr << oss.str() << std::endl;

  processor.Encode(str, &pieces, true, true);

  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  std::cerr << oss.str() << std::endl;

  processor.Encode(str, &pieces, false, false);

  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  std::cerr << oss.str() << std::endl;

  processor.Encode(str, &pieces, false, true);

  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  std::cerr << oss.str() << std::endl;
}

TEST(PinyinEncoder, TestLoad) {
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin.raw";
  auto start = std::chrono::high_resolution_clock::now();
  PinyinEncoder processor(vocab_path);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "Build from text file : "
            << static_cast<int32_t>(duration.count()) << std::endl;

  processor.Save("cppinyin/python/cppinyin/resources/pinyin.dict");

  start = std::chrono::high_resolution_clock::now();

  processor.Load("cppinyin/python/cppinyin/resources/pinyin.dict");

  stop = std::chrono::high_resolution_clock::now();
  duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "Build from binary file : "
            << static_cast<int32_t>(duration.count()) << std::endl;

  std::string str = "我是中国 人我爱我的 love you 祖国";
  std::vector<std::string> pieces;
  processor.Encode(str, &pieces);

  std::ostringstream oss;
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  std::cerr << oss.str() << std::endl;

  processor.Encode(str, &pieces, true, true);

  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  std::cerr << oss.str() << std::endl;

  processor.Encode(str, &pieces, false, false);

  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  std::cerr << oss.str() << std::endl;

  processor.Encode(str, &pieces, false, true);

  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  std::cerr << oss.str() << std::endl;
}

} // namespace cppinyin
