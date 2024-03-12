/**
 * Copyright      2024    Wei Kang (wkang@pku.edu.cn)
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

#include "ssentencepiece/python/csrc/ssentencepiece.h"
#include "ssentencepiece/csrc/ssentencepiece.h"
#include <memory>
#include <string>
#include <vector>

namespace ssentencepiece {

void PybindSsentencepiece(py::module &m) {
  using PyClass = Ssentencepiece;
  py::class_<PyClass>(m, "ssentencepiece")
      .def(
          py::init([](int32_t num_threads = std::thread::hardware_concurrency())
                       -> std::unique_ptr<PyClass> {
            return std::make_unique<PyClass>(num_threads);
          }),
          py::arg("num_threads") = std::thread::hardware_concurrency(),
          py::call_guard<py::gil_scoped_release>())
      .def(
          py::init([](const std::string &vocab_path,
                      int32_t num_threads = std::thread::hardware_concurrency())
                       -> std::unique_ptr<PyClass> {
            return std::make_unique<PyClass>(vocab_path, num_threads);
          }),
          py::arg("vocab_path"),
          py::arg("num_threads") = std::thread::hardware_concurrency(),
          py::call_guard<py::gil_scoped_release>())
      .def(
          "build",
          [](PyClass &self, const std::string &vocab_path) {
            self.Build(vocab_path);
          },
          py::arg("vocab_path"), py::call_guard<py::gil_scoped_release>())
      .def(
          "encode",
          [](PyClass &self, const std::string &str,
             bool output_id = true) -> py::object {
            if (output_id) {
              std::vector<int32_t> oids;
              self.Encode(str, &oids);
              return py::cast(oids);
            } else {
              std::vector<std::string> ostrs;
              self.Encode(str, &ostrs);
              return py::cast(ostrs);
            }
          },
          py::arg("str"), py::arg("output_id") = true,
          py::call_guard<py::gil_scoped_release>())
      .def(
          "encode",
          [](PyClass &self, const std::vector<std::string> &strs,
             bool output_id = true) -> py::object {
            if (output_id) {
              std::vector<std::vector<int32_t>> oids;
              self.Encode(strs, &oids);
              return py::cast(oids);
            } else {
              std::vector<std::vector<std::string>> ostrs;
              self.Encode(strs, &ostrs);
              return py::cast(ostrs);
            }
          },
          py::arg("strs"), py::arg("output_id") = true,
          py::call_guard<py::gil_scoped_release>())
      .def(
          "decode",
          [](PyClass &self, const std::vector<int32_t> &ids) -> py::str {
            std::string res;
            {
              py::gil_scoped_release release;
              res = self.Decode(ids);
            }
            return py::str(
                PyUnicode_DecodeUTF8(res.c_str(), res.size(), "ignore"));
          },
          py::arg("ids"))
      .def(
          "decode",
          [](PyClass &self,
             const std::vector<std::vector<int32_t>> &ids) -> py::list {
            std::vector<std::string> res;
            {
              py::gil_scoped_release release;
              res = self.Decode(ids);
            }
            py::list result;
            for (const auto &r : res) {
              result.append(
                  py::str(PyUnicode_DecodeUTF8(r.c_str(), r.size(), "ignore")));
            }
            return result;
          },
          py::arg("ids"));
}

PYBIND11_MODULE(_ssentencepiece, m) {
  m.doc() = "Python wrapper for simple sentencepiece.";

  PybindSsentencepiece(m);
}

} // namespace ssentencepiece
