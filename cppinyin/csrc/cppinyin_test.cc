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
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin.dict";
  PinyinEncoder processor(vocab_path);

  std::string str = "我是中国 人我爱我的 love you 祖国";

  std::ostringstream oss;
  std::vector<std::string> pieces;

  processor.Encode(str, &pieces);
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "wǒ shì zhōng guó rén wǒ ài wǒ de love you zǔ guó ");

  processor.Encode(str, &pieces, true, true);
  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "w ǒ sh ì zh ōng g uó r én w ǒ ài w ǒ d e love you z ǔ g uó ");

  processor.Encode(str, &pieces, false, false);
  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "wo shi zhong guo ren wo ai wo de love you zu guo ");

  processor.Encode(str, &pieces, false, true);
  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "w o sh i zh ong g uo r en w o ai w o d e love you z u g uo ");
}

TEST(PinyinEncoder, TestSaveLoad) {
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin.raw";
  auto start = std::chrono::high_resolution_clock::now();
  PinyinEncoder processor(vocab_path);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cerr << "Build from text file : "
            << static_cast<int32_t>(duration.count()) << std::endl;

  processor.Save("/tmp/pinyin.dict");

  start = std::chrono::high_resolution_clock::now();

  processor.Load("/tmp/pinyin.dict");

  stop = std::chrono::high_resolution_clock::now();
  duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cerr << "Build from binary file : "
            << static_cast<int32_t>(duration.count()) << std::endl;

  std::string str = "我是中国 人我爱我的 love you 祖国";
  std::vector<std::string> pieces;
  processor.Encode(str, &pieces);

  std::ostringstream oss;
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "wǒ shì zhōng guó rén wǒ ài wǒ de love you zǔ guó ");
}

} // namespace cppinyin
