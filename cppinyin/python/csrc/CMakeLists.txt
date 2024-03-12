pybind11_add_module(_cppinyin
  cppinyin.cc
)

target_link_libraries(_cppinyin PRIVATE cppinyin_core)

install(TARGETS _cppinyin DESTINATION ${CMAKE_INSTALL_PREFIX})
