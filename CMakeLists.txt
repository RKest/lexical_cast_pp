cmake_minimum_required(VERSION 3.25)
project(lexical_cast_pp)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_library(project_warnings INTERFACE include/lpp/lexical_cast.hpp)
    include(cmake/warnings.cmake)
    set_project_warnings(project_warnings)
endif ()

set(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra")

include(cmake/conan.cmake)
run_conan()

include(GoogleTest)
add_subdirectory(tests)
