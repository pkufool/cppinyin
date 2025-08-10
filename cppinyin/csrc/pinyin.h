/**
 * Copyright      2025    Wei Kang (wkang@pku.edu.cn)
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

#ifndef CPPINYIN_CSRC_PINYIN_H_
#define CPPINYIN_CSRC_PINYIN_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace cppinyin {

extern const std::string INITIALS;
extern const std::string PHONETICS;
extern const std::unordered_map<std::string, std::string> PHONETICS_MAP;
extern const std::unordered_map<std::string, std::string> NORMAL_TO_TONE;
extern const std::vector<std::string> PINYIN_DICT;

} // namespace cppinyin

#endif // CPPINYIN_CSRC_PINYIN_H_
