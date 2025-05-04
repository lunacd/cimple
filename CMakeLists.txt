cmake_minimum_required(VERSION 3.30)
project(cps-build LANGUAGES CXX)

add_subdirectory(src)
add_subdirectory(headers)

if (BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
endif ()
