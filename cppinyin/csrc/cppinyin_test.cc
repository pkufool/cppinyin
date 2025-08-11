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
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin.raw";
  PinyinEncoder processor(vocab_path);

  std::string str = "我是中国 人我爱我的 love you 祖国";

  std::ostringstream oss;
  std::vector<std::string> pieces;

  processor.Encode(str, &pieces, "number", false);
  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "wo3 shi4 zhong1 guo2 ren2 wo3 ai4 wo3 de love you zu3 guo2 ");

  processor.Encode(str, &pieces, "number", true);
  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(
      oss.str(),
      "w o3 sh i4 zh ong1 g uo2 r en2 w o3 ai4 w o3 d e love you z u3 g uo2 ");

  processor.Encode(str, &pieces, "normal", false);
  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "wǒ shì zhōng guó rén wǒ ài wǒ de love you zǔ guó ");

  processor.Encode(str, &pieces, "normal", true);
  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "w ǒ sh ì zh ōng g uó r én w ǒ ài w ǒ d e love you z ǔ g uó ");

  processor.Encode(str, &pieces, "none", false);
  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "wo shi zhong guo ren wo ai wo de love you zu guo ");

  processor.Encode(str, &pieces, "none", true);
  oss.str("");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "w o sh i zh ong g uo r en w o ai w o d e love you z u g uo ");
}

TEST(PinyinEncoder, TestEncodeBatch) {
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin.raw";
  PinyinEncoder processor(vocab_path);

  std::vector<std::string> strs({"我是中国 人我爱我的 love you 祖国",
                                 "love you 祖国 我是中国 人我爱我的"});

  std::ostringstream oss;
  std::vector<std::vector<std::string>> pieces;

  processor.Encode(strs, &pieces, "number", false);
  oss.str("");
  for (auto piece : pieces[0]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "wo3 shi4 zhong1 guo2 ren2 wo3 ai4 wo3 de love you zu3 guo2 ");
  oss.str("");
  for (auto piece : pieces[1]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "love you zu3 guo2 wo3 shi4 zhong1 guo2 ren2 wo3 ai4 wo3 de ");

  processor.Encode(strs, &pieces, "number", true);
  oss.str("");
  for (auto piece : pieces[0]) {
    oss << piece << " ";
  }
  EXPECT_EQ(
      oss.str(),
      "w o3 sh i4 zh ong1 g uo2 r en2 w o3 ai4 w o3 d e love you z u3 g uo2 ");
  oss.str("");
  for (auto piece : pieces[1]) {
    oss << piece << " ";
  }
  EXPECT_EQ(
      oss.str(),
      "love you z u3 g uo2 w o3 sh i4 zh ong1 g uo2 r en2 w o3 ai4 w o3 d e ");

  processor.Encode(strs, &pieces, "normal", false);
  oss.str("");
  for (auto piece : pieces[0]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "wǒ shì zhōng guó rén wǒ ài wǒ de love you zǔ guó ");

  oss.str("");
  for (auto piece : pieces[1]) {
    oss << piece << " ";
  }

  EXPECT_EQ(oss.str(), "love you zǔ guó wǒ shì zhōng guó rén wǒ ài wǒ de ");

  processor.Encode(strs, &pieces, "normal", true);
  oss.str("");
  for (auto piece : pieces[0]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "w ǒ sh ì zh ōng g uó r én w ǒ ài w ǒ d e love you z ǔ g uó ");
  oss.str("");
  for (auto piece : pieces[1]) {
    oss << piece << " ";
  }

  EXPECT_EQ(oss.str(),
            "love you z ǔ g uó w ǒ sh ì zh ōng g uó r én w ǒ ài w ǒ d e ");

  processor.Encode(strs, &pieces, "none", false);
  oss.str("");
  for (auto piece : pieces[0]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "wo shi zhong guo ren wo ai wo de love you zu guo ");
  oss.str("");
  for (auto piece : pieces[1]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(), "love you zu guo wo shi zhong guo ren wo ai wo de ");

  processor.Encode(strs, &pieces, "none", true);
  oss.str("");
  for (auto piece : pieces[0]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "w o sh i zh ong g uo r en w o ai w o d e love you z u g uo ");
  oss.str("");
  for (auto piece : pieces[1]) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "love you z u g uo w o sh i zh ong g uo r en w o ai w o d e ");
}

TEST(PinyinEncoder, TestLoadFromNormal) {
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin.raw";
  PinyinEncoder processor(vocab_path);

  std::string str = "我是中国 人我爱我的 love you 祖国";

  std::ostringstream oss;
  std::vector<std::string> pieces;

  processor.Encode(str, &pieces, "number");
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "wo3 shi4 zhong1 guo2 ren2 wo3 ai4 wo3 de love you zu3 guo2 ");
}

TEST(PinyinEncoder, TestToInitialToFinal) {
  PinyinEncoder processor;
  std::vector<std::string> pinyins = {"wǒ",  "shì", "zhōng", "guó", "rén",
                                      "wǒ",  "ài",  "wǒ",    "de",  "love",
                                      "you", "zǔ",  "guó"};
  std::vector<std::string> pinyin_numbers = {
      "wo3", "shi4", "zhong1", "guo2", "ren2", "wo3", "ai4",
      "wo3", "de",   "love",   "you",  "zu3",  "guo2"};
  std::vector<std::string> initials = {"w", "sh", "zh", "g", "r", "w", "",
                                       "w", "d",  "",   "",  "z", "g"};
  std::vector<std::string> finals = {"ǒ", "ì", "ōng", "uó", "én", "ǒ", "ài",
                                     "ǒ", "e", "",    "",   "ǔ",  "uó"};
  std::vector<std::string> finals_numbers = {"o3", "i4",  "ong1", "uo2", "en2",
                                             "o3", "ai4", "o3",   "e",   "",
                                             "",   "u3",  "uo2"};
  std::vector<std::string> finals_none = {
      "o", "i", "ong", "uo", "en", "o", "ai", "o", "e", "", "", "u", "uo"};

  std::vector<std::string> res;
  processor.ToInitials(pinyins, &res);
  for (int32_t i = 0; i < initials.size(); ++i) {
    EXPECT_EQ(res[i], initials[i]);
  }

  processor.ToInitials(pinyin_numbers, &res);
  for (int32_t i = 0; i < initials.size(); ++i) {
    EXPECT_EQ(res[i], initials[i]);
  }

  processor.ToFinals(pinyins, &res, "normal");
  for (int32_t i = 0; i < finals.size(); ++i) {
    EXPECT_EQ(res[i], finals[i]);
  }

  processor.ToFinals(pinyin_numbers, &res, "normal");
  for (int32_t i = 0; i < finals.size(); ++i) {
    EXPECT_EQ(res[i], finals[i]);
  }

  processor.ToFinals(pinyins, &res, "number");
  for (int32_t i = 0; i < finals_numbers.size(); ++i) {
    EXPECT_EQ(res[i], finals_numbers[i]);
  }

  processor.ToFinals(pinyin_numbers, &res, "number");
  for (int32_t i = 0; i < finals_numbers.size(); ++i) {
    EXPECT_EQ(res[i], finals_numbers[i]);
  }

  processor.ToFinals(pinyins, &res, "none");
  for (int32_t i = 0; i < finals_none.size(); ++i) {
    EXPECT_EQ(res[i], finals_none[i]);
  }

  processor.ToFinals(pinyin_numbers, &res, "none");
  for (int32_t i = 0; i < finals_none.size(); ++i) {
    EXPECT_EQ(res[i], finals_none[i]);
  }
}

TEST(PinyinEncoder, TestSaveLoad) {
  std::string vocab_path = "cppinyin/python/cppinyin/resources/pinyin.raw";
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
  processor_b.Encode(str, &pieces, "number");

  std::ostringstream oss;
  for (auto piece : pieces) {
    oss << piece << " ";
  }
  EXPECT_EQ(oss.str(),
            "wo3 shi4 zhong1 guo2 ren2 wo3 ai4 wo3 de love you zu3 guo2 ");
}

TEST(PinyinEncoder, TestAllPinyin) {
  PinyinEncoder processor;
  std::ostringstream oss;
  auto pinyins = processor.AllPinyin("number", false);
  oss.str("");
  for (const auto &pinyin : pinyins) {
    oss << pinyin << " ";
  }
  std::cerr << "All pinyins in number tone: " << oss.str() << std::endl;

  pinyins = processor.AllPinyin("normal", false);
  oss.str("");
  for (const auto &pinyin : pinyins) {
    oss << pinyin << " ";
  }
  std::cerr << "All pinyins in normal tone: " << oss.str() << std::endl;

  pinyins = processor.AllPinyin("none", false);
  oss.str("");
  for (const auto &pinyin : pinyins) {
    oss << pinyin << " ";
  }
  std::cerr << "All pinyins in none tone: " << oss.str() << std::endl;

  pinyins = processor.AllPinyin("number", true);
  oss.str("");
  for (const auto &pinyin : pinyins) {
    oss << pinyin << " ";
  }
  std::cerr << "All partial pinyins in number tone: " << oss.str() << std::endl;

  pinyins = processor.AllPinyin("normal", true);
  oss.str("");
  for (const auto &pinyin : pinyins) {
    oss << pinyin << " ";
  }
  std::cerr << "All partial pinyins in normal tone: " << oss.str() << std::endl;

  pinyins = processor.AllPinyin("none", true);
  oss.str("");
  for (const auto &pinyin : pinyins) {
    oss << pinyin << " ";
  }
  std::cerr << "All partial pinyins in none tone: " << oss.str() << std::endl;
}

TEST(PinyinEncoder, TestAllInitialFinals) {
  PinyinEncoder processor;
  std::ostringstream oss;
  auto pinyins = processor.AllInitials();
  oss.str("");
  for (const auto &pinyin : pinyins) {
    oss << pinyin << " ";
  }
  std::cerr << "All pinyin initials: " << oss.str() << std::endl;

  pinyins = processor.AllFinals("normal");
  oss.str("");
  for (const auto &pinyin : pinyins) {
    oss << pinyin << " ";
  }
  std::cerr << "All pinyin finals in normal tone: " << oss.str() << std::endl;

  pinyins = processor.AllFinals("none");
  oss.str("");
  for (const auto &pinyin : pinyins) {
    oss << pinyin << " ";
  }
  std::cerr << "All pinyin finals in none tone: " << oss.str() << std::endl;

  pinyins = processor.AllFinals("number");
  oss.str("");
  for (const auto &pinyin : pinyins) {
    oss << pinyin << " ";
  }
  std::cerr << "All partial pinyin finals in number tone: " << oss.str()
            << std::endl;
}

TEST(PinyinEncoder, TestValidPinyin) {
  PinyinEncoder processor;
  std::vector<std::string> pinyins = {
      "wǒ",   "shì", "zhōng", "guó", "rén", "wǒ",   "ài",     "wǒ",   "de",
      "love", "you", "zǔ",    "guó", "wo3", "shi4", "zhong1", "guo2", "ren2",
      "wo3",  "ai4", "wo3",   "de",  "zu3", "guo2", "wo",     "shi",  "zhong",
      "guo",  "ren", "wo",    "ai",  "wo",  "de",   "zu",     "guo"};
  std::vector<bool> valids = {
      true, true, true, true, true, true, true, true, true, false, true, true,
      true, true, true, true, true, true, true, true, true, true,  true, true,
      true, true, true, true, true, true, true, true, true, true,  true};

  for (int32_t i = 0; i < pinyins.size(); ++i) {
    EXPECT_EQ(processor.ValidPinyin(pinyins[i]), valids[i]);
  }

  pinyins = {"wǒ", "shì", "zhōng", "guó", "rén", "wǒ", "ài",
             "wǒ", "de",  "love",  "you", "zǔ",  "guó"};
  valids = {true, true, true,  true,  true, true, true,
            true, true, false, false, true, true};

  for (int32_t i = 0; i < pinyins.size(); ++i) {
    EXPECT_EQ(processor.ValidPinyin(pinyins[i], "normal"), valids[i]);
  }

  pinyins = {"wo3", "shi4", "zhong1", "guo2", "ren2", "wo3", "ai4",
             "wo3", "de",   "love",   "you",  "zu3",  "guo2"};
  valids = {true, true, true,  true,  true, true, true,
            true, true, false, false, true, true};

  for (int32_t i = 0; i < pinyins.size(); ++i) {
    EXPECT_EQ(processor.ValidPinyin(pinyins[i], "number"), valids[i]);
  }

  pinyins = {"wo", "shi", "zhong", "guo", "ren", "wo", "ai",
             "wo", "de",  "love",  "you", "zu",  "guo"};
  valids = {true, true, true,  true, true, true, true,
            true, true, false, true, true, true};

  for (int32_t i = 0; i < pinyins.size(); ++i) {
    EXPECT_EQ(processor.ValidPinyin(pinyins[i], "none"), valids[i]);
  }
}

} // namespace cppinyin
