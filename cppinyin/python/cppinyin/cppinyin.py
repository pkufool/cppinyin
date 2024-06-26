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

import _cppinyin
import os
from typing import List, Union

import importlib_resources


class Encoder:
    def __init__(self, vocab: str = None, num_threads: int = os.cpu_count()):
        """
        Construct a cppinyin Encoder object.
        """
        if vocab is None:
            ref = (
                importlib_resources.files("cppinyin") / "resources/pinyin.dict"
            )
            with importlib_resources.as_file(ref) as path:
                vocab = str(path)

        self.encoder = _cppinyin.Encoder(vocab, num_threads)

    def encode(
        self,
        data: Union[str, List[str]],
        tone: bool = True,
        partial: bool = False,
    ):
        return self.encoder.encode(data, tone, partial)

    def load(self, path: str):
        self.encoder.load(path)

    def save(self, path: str):
        self.encoder.save(path)
