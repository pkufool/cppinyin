function(cppinyin_add_py_test source)
  get_filename_component(name ${source} NAME_WE)
  set(name "${name}_py")

  add_test(NAME ${name}
    COMMAND
      "${PYTHON_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/${source}"
    WORKING_DIRECTORY
      ${CMAKE_CURRENT_SOURCE_DIR}
  )

  get_filename_component(cppinyin_path ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)

  set_property(TEST ${name}
    PROPERTY ENVIRONMENT "PYTHONPATH=${cppinyin_path}:$<TARGET_FILE_DIR:_cppinyin>:$ENV{PYTHONPATH}"
  )
endfunction()

# please sort the files in alphabetic order
set(py_test_files
  test_cppinyin.py
)

foreach(source IN LISTS py_test_files)
  cppinyin_add_py_test(${source})
endforeach()

