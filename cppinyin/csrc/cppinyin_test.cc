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

#include "ssentencepiece/csrc/ssentencepiece.h"

namespace ssentencepiece {

TEST(Ssentencepiece, TestEncode) {
  std::string vocab_path = "ssentencepiece/python/tests/testdata/bpe.vocab";
  Ssentencepiece processor(vocab_path);

  std::string str = "HELLO WORLD  CHINA";
  std::vector<std::string> pieces;
  processor.Encode(str, &pieces);

  std::ostringstream oss;
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "▁HE LL O ▁WORLD ▁ CH IN A ");

  std::vector<int32_t> ids;
  processor.Encode(str, &ids);
  oss.str("");
  oss.clear();
  for (auto id : ids) {
    oss << id << " ";
  }
  EXPECT_EQ(oss.str(), "22 58 24 425 34 81 40 20 ");

  std::vector<std::string> strs(
      {"HELLO  WORLD CHINESE", "I LOVE BEIJING", "LOVE YOU AMERICAN"});

  std::vector<std::string> expect_strs({"▁HE LL O ▁WORLD ▁C HI NE SE ",
                                        "▁I ▁LOVE ▁BE I J ING ",
                                        "▁LOVE ▁YOU ▁A M ER IC AN "});
  std::vector<std::vector<std::string>> opieces;
  processor.Encode(strs, &opieces);

  for (int32_t i = 0; i < opieces.size(); ++i) {
    oss.str("");
    oss.clear();
    for (auto &piece : opieces[i]) {
      oss << piece << " ";
    }
    EXPECT_EQ(oss.str(), expect_strs[i]);
  }

  std::vector<std::string> expect_ids({"22 58 24 425 39 233 88 77 ",
                                       "19 370 37 27 340 14 ",
                                       "370 47 5 17 29 93 143 "});
  std::vector<std::vector<int32_t>> oids;
  processor.Encode(strs, &oids);

  for (int32_t i = 0; i < oids.size(); ++i) {
    oss.str("");
    oss.clear();
    for (auto &id : oids[i]) {
      oss << id << " ";
    }
    EXPECT_EQ(oss.str(), expect_ids[i]);
  }
}

TEST(Ssentencepiece, TestDecode) {
  std::string vocab_path = "ssentencepiece/python/tests/testdata/bpe.vocab";
  Ssentencepiece processor(vocab_path);

  std::vector<int32_t> ids({22, 58, 24, 425, 34, 81, 40, 20});
  std::string res = processor.Decode(ids);
  EXPECT_EQ(res, "HELLO WORLD CHINA");

  std::vector<std::vector<int32_t>> ids_arr({{22, 58, 24, 425, 39, 233, 88, 77},
                                             {19, 370, 37, 27, 340, 14},
                                             {370, 47, 5, 17, 29, 93, 143}});
  std::vector<std::string> res_arr = processor.Decode(ids_arr);
  std::vector<std::string> expect_strs(
      {"HELLO WORLD CHINESE", "I LOVE BEIJING", "LOVE YOU AMERICAN"});
  for (int32_t i = 0; i < res_arr.size(); ++i) {
    EXPECT_EQ(res_arr[i], expect_strs[i]);
  }
}

TEST(Ssentencepiece, TestEncodeBbpe) {
  std::string vocab_path = "ssentencepiece/python/tests/testdata/bbpe.vocab";
  Ssentencepiece processor(vocab_path);

  std::string str = "中华巍巍";
  std::vector<std::string> pieces;
  processor.Encode(str, &pieces);

  std::ostringstream oss;
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "▁中 华 <0xE5> <0xB7> <0x8D> <0xE5> <0xB7> <0x8D> ");

  std::vector<int32_t> ids;
  processor.Encode(str, &ids);
  oss.str("");
  oss.clear();
  for (auto id : ids) {
    oss << id << " ";
  }
  EXPECT_EQ(oss.str(), "326 1433 232 186 144 232 186 144 ");
}

TEST(Ssentencepiece, TestDecodeBbpe) {
  std::string vocab_path = "ssentencepiece/python/tests/testdata/bbpe.vocab";
  Ssentencepiece processor(vocab_path);

  std::vector<int32_t> ids({326, 1433, 232, 186, 144, 232, 186, 144});
  std::string res = processor.Decode(ids);
  EXPECT_EQ(res, "中华巍巍");
}

} // namespace ssentencepiece
