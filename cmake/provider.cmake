cmake_policy(SET CMP0135 NEW)

include(FetchContent)
FetchContent_Declare(
    cps
    GIT_REPOSITORY https://github.com/cps-org/cps-config.git
    GIT_TAG dceec9a1efed8b13f2a34111fe40cdd9a18edd99
)
FetchContent_Declare(
    GTest
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.tar.gz
    URL_HASH SHA256=78c676fc63881529bf97bf9d45948d905a66833fbfa5318ea2cd7478cb98f399
)
FetchContent_Declare(
    tl-expected
    URL https://github.com/TartanLlama/expected/archive/v1.1.0.tar.gz
    URL_HASH SHA256=1db357f46dd2b24447156aaf970c4c40a793ef12a8a9c2ad9e096d9801368df6
)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/archive/refs/tags/v3.12.0.tar.gz
    URL_HASH SHA256=4B92EB0C06D10683F7447CE9406CB97CD4B453BE18D7279320F7B2F025C10187
)
FetchContent_Declare(
    fmt
    URL https://github.com/fmtlib/fmt/archive/10.1.1.tar.gz
    URL_HASH SHA256=78b8c0a72b1c35e4443a7e308df52498252d1cefc2b08c9a97bc9ee6cfe61f8b
)
FetchContent_Declare(
    CLI11
    URL https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.5.0.tar.gz
    URL_HASH SHA256=17E02B4CDDC2FA348E5DBDBB582C59A3486FA2B2433E70A0C3BACB871334FD55
)
FetchContent_Declare(
    cpp-subprocess
    GIT_REPOSITORY https://github.com/lunacd/cpp-subprocess.git
    GIT_TAG 27ac26ca4be64213820e220ae8a5b7dea08125c4
)

# Skip unneeded parts in graaf
set(SKIP_TESTS ON)
set(SKIP_BENCHMARKS ON)
set(SKIP_EXAMPLES ON)

# Configure Boost
set(BOOST_INCLUDE_LIBRARIES process)
set(BOOST_ENABLE_CMAKE ON)

macro(provide_dependency method dep_name)
    if ("${dep_name}" MATCHES "^(cps|GTest|tl-expected|nlohmann_json|fmt|CLI11|cpp-subprocess)$")
        FetchContent_MakeAvailable(${dep_name})
        set(${dep_name}_FOUND TRUE)
    endif()
endmacro()

cmake_language(
    SET_DEPENDENCY_PROVIDER provide_dependency
    SUPPORTED_METHODS
        FIND_PACKAGE
)
