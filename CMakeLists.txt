cmake_minimum_required(VERSION 3.31)
project(cps-build LANGUAGES CXX)

add_subdirectory(src)
add_subdirectory(headers)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
