include(CTest)

find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS process)

add_executable(cimple-tests
    basic.cpp
    test_util.cpp
)

target_sources(cimple-tests PRIVATE FILE_SET HEADERS FILES
    test_util.hpp
)

target_link_libraries(cimple-tests PRIVATE
    GTest::gtest
    GTest::gmock
    GTest::gmock_main
    libcimple
    Boost::process
)

if (WIN32)
    target_link_libraries(cimple-tests PRIVATE wsock32 bcrypt)
endif()

target_compile_definitions(cimple-tests PRIVATE BOOST_PROCESS_USE_STD_FS)

include(GoogleTest)
gtest_discover_tests(cimple-tests
    ENVIRONMENT_MODIFICATION
    "CIMPLE_TEST_DATA_DIR=set:${CMAKE_CURRENT_SOURCE_DIR}"
)
