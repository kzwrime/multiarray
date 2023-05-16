cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    multiarray
    VERSION 0.1.0
    DESCRIPTION "A simple C++ templated multiarray class for array, a header-only library"
    HOMEPAGE_URL "https://github.com/kzwrime/multiarray"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(multiarray_multiarray INTERFACE)
add_library(multiarray::multiarray ALIAS multiarray_multiarray)

set_property(
    TARGET multiarray_multiarray PROPERTY
    EXPORT_NAME multiarray
)

target_include_directories(
    multiarray_multiarray ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(multiarray_multiarray INTERFACE cxx_std_17)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${multiarray_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT multiarray_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of multiarray"
  )
endif()

include(cmake/dev-mode.cmake)
