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

#include "cppinyin/python/csrc/cppinyin.h"
#include "cppinyin/csrc/cppinyin.h"
#include <memory>
#include <string>
#include <vector>

namespace cppinyin {

void PybindCppinyin(py::module &m) {
  using PyClass = PinyinEncoder;
  py::class_<PyClass>(m, "Encoder")
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
          "load",
          [](PyClass &self, const std::string &vocab_path) {
            self.Load(vocab_path);
          },
          py::arg("vocab_path"), py::call_guard<py::gil_scoped_release>())
      .def(
          "save",
          [](PyClass &self, const std::string &vocab_path) {
            self.Save(vocab_path);
          },
          py::arg("vocab_path"), py::call_guard<py::gil_scoped_release>())
      .def(
          "encode",
          [](PyClass &self, const std::string &str, const std::string &tone,
             bool partial, bool return_seg) -> py::object {
            std::vector<std::string> ostrs;
            std::vector<std::string> osegs;
            {
              py::gil_scoped_release release;
              self.Encode(str, &ostrs, tone, partial, &osegs);
            }
            if (return_seg) {
              return py::make_tuple(py::cast(ostrs), py::cast(osegs));
            } else {
              return py::cast(ostrs);
            }
          },
          py::arg("str"), py::arg("tone") = "number",
          py::arg("partial") = false, py::arg("return_seg") = false)
      .def(
          "encode",
          [](PyClass &self, const std::vector<std::string> &strs,
             const std::string &tone, bool partial,
             bool return_seg) -> py::object {
            std::vector<std::vector<std::string>> ostrs;
            std::vector<std::vector<std::string>> osegs;
            {
              py::gil_scoped_release release;
              self.Encode(strs, &ostrs, tone, partial, &osegs);
            }
            if (return_seg) {
              return py::make_tuple(py::cast(ostrs), py::cast(osegs));
            } else {
              return py::cast(ostrs);
            }
          },
          py::arg("strs"), py::arg("tone") = "number",
          py::arg("partial") = false, py::arg("return_seg") = false)
      .def(
          "to_initials",
          [](PyClass &self, const std::string &str) -> std::string {
            py::gil_scoped_release release;
            return self.ToInitial(str);
          },
          py::arg("str"))
      .def(
          "to_initials",
          [](PyClass &self,
             const std::vector<std::string> &strs) -> std::vector<std::string> {
            std::vector<std::string> ostrs;
            py::gil_scoped_release release;
            self.ToInitials(strs, &ostrs);
            return ostrs;
          },
          py::arg("strs"))
      .def(
          "to_final",
          [](PyClass &self, const std::string &str,
             const std::string &tone = "number") -> std::string {
            py::gil_scoped_release release;
            return self.ToFinal(str, tone);
          },
          py::arg("str"), py::arg("tone") = "number")
      .def(
          "to_finals",
          [](PyClass &self, const std::vector<std::string> &strs,
             const std::string &tone = "number") -> std::vector<std::string> {
            std::vector<std::string> ostrs;
            py::gil_scoped_release release;
            self.ToFinals(strs, &ostrs, tone);
            return ostrs;
          },
          py::arg("strs"), py::arg("tone") = "number")
      .def(
          "valid_pinyin",
          [](PyClass &self, const std::string &str,
             const std::string &tone = "number") -> bool {
            py::gil_scoped_release release;
            bool res = self.ValidPinyin(str, tone);
            return res;
          },
          py::arg("str"), py::arg("tone") = "number")

      .def(
          "all_pinyins",
          [](PyClass &self, const std::string &tone = "number",
             bool partial = false) -> std::vector<std::string> {
            py::gil_scoped_release release;
            std::vector<std::string> ostrs = self.AllPinyin(tone, partial);
            return ostrs;
          },
          py::arg("tone") = "number", py::arg("partial") = false)
      .def("all_initials",
           [](PyClass &self) -> std::vector<std::string> {
             py::gil_scoped_release release;
             std::vector<std::string> ostrs = self.AllInitials();
             return ostrs;
           })
      .def(
          "all_finals",
          [](PyClass &self,
             const std::string &tone = "number") -> std::vector<std::string> {
            py::gil_scoped_release release;
            std::vector<std::string> ostrs = self.AllFinals(tone);
            return ostrs;
          },
          py::arg("tone") = "number");
}

PYBIND11_MODULE(_cppinyin, m) {
  m.doc() = "Python wrapper for Chinese to pinyin.";

  PybindCppinyin(m);
}

} // namespace cppinyin
