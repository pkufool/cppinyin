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
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cppinyin/csrc/cppinyin.h"

namespace cppinyin {

TEST(PinyinEncoder, TestEncode) {
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin_v1.txt";
  PinyinEncoder processor(vocab_path);

  std::string str = "我是中国 人我爱我的 love you 祖国";

  std::ostringstream oss;
  std::vector<std::string> pieces;
  std::vector<std::string> segs;

  processor.Encode(str, &pieces);
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "wo3 shi4 zhong1 guo2 ren2 wo3 ai4 wo3 de love you zu3 guo2 ");

  processor.Encode(str, &pieces, true, true, &segs);
  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }

  EXPECT_EQ(
      oss.str(),
      "w o3 sh i4 zh ong1 g uo2 r en2 w o3 ai4 w o3 d e love you z u3 g uo2 ");

  oss.str("");
  for (auto seg : segs) {
    oss << seg << " ";
  }
  std::cout << oss.str() << std::endl;

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

TEST(PinyinEncoder, TestEncodeBatch) {
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin_v1.txt";
  PinyinEncoder processor(vocab_path);

  std::vector<std::string> strs({"我是中国 人我爱我的 love you 祖国",
                                 "love you 祖国 我是中国 人我爱我的"});

  std::ostringstream oss;
  std::vector<std::vector<std::string>> pieces;
  std::vector<std::vector<std::string>> segs;

  processor.Encode(strs, &pieces);
  for (auto piece : pieces[0]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "wo3 shi4 zhong1 guo2 ren2 wo3 ai4 wo3 de love you zu3 guo2 ");

  processor.Encode(strs, &pieces, true, true, &segs);
  oss.str("");
  for (auto piece : pieces[0]) {
    oss << piece << " ";
  }

  EXPECT_EQ(
      oss.str(),
      "w o3 sh i4 zh ong1 g uo2 r en2 w o3 ai4 w o3 d e love you z u3 g uo2 ");

  oss.str("");
  for (auto piece : segs[1]) {
    oss << piece << " ";
  }
  std::cout << oss.str() << std::endl;

  processor.Encode(strs, &pieces, false, false);
  oss.str("");
  for (auto piece : pieces[0]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "wo shi zhong guo ren wo ai wo de love you zu guo ");

  processor.Encode(strs, &pieces, false, true);
  oss.str("");
  for (auto piece : pieces[0]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "w o sh i zh ong g uo r en w o ai w o d e love you z u g uo ");
}

TEST(PinyinEncoder, TestLoadFromNormal) {
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin.raw";
  PinyinEncoder processor(vocab_path);

  std::string str = "我是中国 人我爱我的 love you 祖国";

  std::ostringstream oss;
  std::vector<std::string> pieces;

  processor.Encode(str, &pieces);
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "wo3 shi4 zhong1 guo2 ren2 wo3 ai4 wo3 de love you zu3 guo2 ");
}

TEST(PinyinEncoder, TestSaveLoad) {
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin_v1.txt";
  auto start = std::chrono::high_resolution_clock::now();
  PinyinEncoder processor_t(vocab_path);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cerr << "Build from text file : "
            << static_cast<int32_t>(duration.count()) << std::endl;

  processor_t.Save("/tmp/pinyin.dict");

  start = std::chrono::high_resolution_clock::now();

  PinyinEncoder processor_b("/tmp/pinyin.dict");

  stop = std::chrono::high_resolution_clock::now();
  duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cerr << "Build from binary file : "
            << static_cast<int32_t>(duration.count()) << std::endl;

  std::string str = "我是中国 人我爱我的 love you 祖国";
  std::vector<std::string> pieces;
  processor_b.Encode(str, &pieces);

  std::ostringstream oss;
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "wo3 shi4 zhong1 guo2 ren2 wo3 ai4 wo3 de love you zu3 guo2 ");
}

} // namespace cppinyin
