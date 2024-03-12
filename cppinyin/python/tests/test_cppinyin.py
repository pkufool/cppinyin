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
#  ctest --verbose -R ssentencepiece_test_py


import unittest
import numpy as np

from ssentencepiece import Ssentencepiece
import sentencepiece as spm


class TestEncodeDecode(unittest.TestCase):
    def test_encode_decode(self):
        sp = spm.SentencePieceProcessor()
        sp.load("ssentencepiece/python/tests/testdata/bpe.model")
        ssp = Ssentencepiece("ssentencepiece/python/tests/testdata/bpe.vocab")
        sentences = [
            "Licensed under the Apache License Version the License",
            "you may not use this file except in compliance with the License",
            "You may obtain a copy of the License at",
            "Unless required by applicable law or agreed to in writing software",
            "distributed under the License is distributed on an AS IS BASIS",
            "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied",
            "See the License for the specific language governing permissions and",
        ]
        sentences = [x.upper() for x in sentences]
        for sen in sentences:
            r1 = sp.encode(sen)
            r2 = ssp.encode(sen)
            assert r1 == r2, (r1, r2)
            r1 = sp.encode(sen, out_type=str)
            r2 = ssp.encode(sen, out_type=str)
            assert r1 == r2, (r1, r2)
            r1 = sp.decode(sp.encode(sen))
            r2 = ssp.decode(ssp.encode(sen))
            assert r1 == r2, (r1, r2)

        r1 = sp.encode(sentences)
        r2 = ssp.encode(sentences)
        assert r1 == r2, (r1, r2)
        r1 = sp.decode(sp.encode(sentences))
        r2 = ssp.decode(ssp.encode(sentences))
        assert r1 == r2, (r1, r2)

    def test_encode_decode_bbpe(self):
        sp = spm.SentencePieceProcessor()
        sp.load("ssentencepiece/python/tests/testdata/bbpe.model")
        ssp = Ssentencepiece("ssentencepiece/python/tests/testdata/bbpe.vocab")
        sentences = [
            "Licensed under the Apache License Version the License",
            "you may not use this file except in compliance with the License",
            "You may obtain a copy of the License at",
            "Unless required by applicable law or agreed to in writing software",
            "distributed under the License is distributed on an AS IS BASIS",
            "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied",
            "See the License for the specific language governing permissions and",
        ]
        sentences = [x.upper() for x in sentences]
        for sen in sentences:
            r1 = sp.encode(sen)
            r2 = ssp.encode(sen)
            assert r1 == r2, (r1, r2)
            r1 = sp.encode(sen, out_type=str)
            r2 = ssp.encode(sen, out_type=str)
            assert r1 == r2, (r1, r2)
            r1 = sp.decode(sp.encode(sen))
            r2 = ssp.decode(ssp.encode(sen))
            assert r1 == r2, (r1, r2)

        r1 = sp.encode(sentences)
        r2 = ssp.encode(sentences)
        assert r1 == r2, (r1, r2)
        r1 = sp.decode(sp.encode(sentences))
        r2 = ssp.decode(ssp.encode(sentences))
        assert r1 == r2, (r1, r2)


if __name__ == "__main__":
    unittest.main()
