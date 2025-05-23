find_package(GTest REQUIRED)

add_executable(cimple-tests
    build_tests/basic.cpp
    build_tests/build_test_util.cpp
    pkg_tests/pkg_test_util.cpp
    pkg_tests/basic.cpp
    test_util.cpp
)

target_sources(cimple-tests PRIVATE FILE_SET HEADERS FILES
    test_util.hpp
    build_tests/build_test_util.hpp
    pkg_tests/pkg_test_util.hpp
)

target_link_libraries(cimple-tests PRIVATE
    GTest::gtest
    GTest::gmock
    GTest::gmock_main
    cimple
    cpp-subprocess::subprocess
)

include(GoogleTest)
gtest_discover_tests(cimple-tests PROPERTIES
    ENVIRONMENT_MODIFICATION
    "CIMPLE_TEST_DATA_DIR=set:${CMAKE_CURRENT_SOURCE_DIR}"
)
