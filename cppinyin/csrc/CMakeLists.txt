set(cppinyin_srcs
  cppinyin.cc
  utils.cc
)

add_library(cppinyin_core ${cppinyin_srcs})

if (NOT WIN32)
target_link_libraries(cppinyin_core pthread)
endif()

install(TARGETS cppinyin_core DESTINATION ${CMAKE_INSTALL_PREFIX})

function(cppinyin_add_test source)
  get_filename_component(name ${source} NAME_WE)
  add_executable(${name} "${source}")
  target_link_libraries(${name}
    PRIVATE
      cppinyin_core
      gtest
      gtest_main
  )

  add_test(NAME "Test.${name}"
    COMMAND
    $<TARGET_FILE:${name}>
    WORKING_DIRECTORY
    ${CMAKE_SOURCE_DIR}
  )
endfunction()

if(CPPINYIN_ENABLE_TESTS)
  # please sort the source files alphabetically
  set(test_srcs
    cppinyin_test.cc
  )

  foreach(source IN LISTS test_srcs)
    cppinyin_add_test(${source})
  endforeach()
endif()
