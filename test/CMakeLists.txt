cmake_minimum_required(VERSION 3.14)

project(multiarrayTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)
include(../cmake/windows-set-path.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(multiarray REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(multiarray_test source/multiarray_test.cpp)
target_link_libraries(multiarray_test PRIVATE multiarray::multiarray)
target_compile_features(multiarray_test PRIVATE cxx_std_17)

add_test(NAME multiarray_test COMMAND multiarray_test)

# ---- End-of-file commands ----

add_folders(Test)
