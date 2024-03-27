#!/usr/bin/env python3
#
# Copyright      2024 Wei Kang (wkang@pku.edu.cn)
#
# See ../../../LICENSE for clarification regarding multiple authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# To run this single test, use
#
#  ctest --verbose -R cppinyin_test_py


import unittest

import cppinyin as cp


class TestEncode(unittest.TestCase):
    def test_encode_decode(self):
        cpp = cp.Encoder("../cppinyin/resources/pinyin.dict")
        phrases = ["一切反动派都是纸老虎", "宜将剩勇追穷寇不可沽名学霸王", "我是中国人民的儿子"]
        pinyins = [
            "yī qiè fǎn dòng pài dōu shì zhǐ lǎo hǔ",
            "yí jiāng shèng yǒng zhuī qióng kòu bù kě gū míng xué bà wáng",
            "wǒ shì zhōng guó rén mín de ér zi",
        ]
        for i, p in enumerate(phrases):
            res = " ".join(cpp.encode(p))
            print(res)
            assert res == pinyins[i], (res, pinyins[i])
        res = cpp.encode(phrases)
        res = [" ".join(x) for x in res]
        assert pinyins == res, (pinyins, res)


if __name__ == "__main__":
    unittest.main()
