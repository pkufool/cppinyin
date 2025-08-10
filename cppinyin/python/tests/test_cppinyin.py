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
        cpp = cp.Encoder("../cppinyin/resources/pinyin.raw")
        phrases = ["一切反动派都是纸老虎", "宜将剩勇追穷寇不可沽名学霸王", "我是中国人民的儿子"]
        pinyins = [
            "yi1 qie4 fan3 dong4 pai4 dou1 shi4 zhi3 lao3 hu3",
            "yi2 jiang1 sheng4 yong3 zhui1 qiong2 kou4 bu4 ke3 gu1 ming2 xue2 ba4 wang2",
            "wo3 shi4 zhong1 guo2 ren2 min2 de er2 zi",
        ]
        for i, p in enumerate(phrases):
            res, seg = cpp.encode(p, return_seg=True)
            res = " ".join(res)
            seg = " ".join(seg)
            print(res)
            print(seg)
            assert res == pinyins[i], (res, pinyins[i])
        res, seg = cpp.encode(phrases, return_seg=True)
        res = [" ".join(x) for x in res]
        print(res)
        print(seg)
        assert pinyins == res, (pinyins, res)


if __name__ == "__main__":
    unittest.main()
