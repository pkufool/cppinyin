function(download_pybind11)

  include(FetchContent)

  set(pybind11_URL  "https://github.com/pybind/pybind11/archive/refs/tags/v2.11.1.zip")
  set(pybind11_HASH "SHA256=b011a730c8845bfc265f0f81ee4e5e9e1d354df390836d2a25880e123d021f89")

  # If you don't have access to the Internet,
  # please pre-download pybind11
  set(possible_file_locations
    $ENV{HOME}/Downloads/pybind11-2.11.1.tar.gz
    ${PROJECT_SOURCE_DIR}/pybind11-2.11.1.tar.gz
    ${PROJECT_BINARY_DIR}/pybind11-2.11.1.tar.gz
    /tmp/pybind11-2.11.1.tar.gz
  )

  foreach(f IN LISTS possible_file_locations)
    if(EXISTS ${f})
      set(pybind11_URL  "${f}")
      file(TO_CMAKE_PATH "${pybind11_URL}" pybind11_URL)
      break()
    endif()
  endforeach()

  FetchContent_Declare(pybind11
    URL
      ${pybind11_URL}
    URL_HASH          ${pybind11_HASH}
  )

  FetchContent_GetProperties(pybind11)
  if(NOT pybind11_POPULATED)
    message(STATUS "Downloading pybind11 from ${pybind11_URL}")
    FetchContent_Populate(pybind11)
  endif()
  message(STATUS "pybind11 is downloaded to ${pybind11_SOURCE_DIR}")
  add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR} EXCLUDE_FROM_ALL)
endfunction()

download_pybind11()
