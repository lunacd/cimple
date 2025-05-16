add_library(cimple
    cimple_config.cpp
    cimple_util.cpp
    cimple_build.cpp
    cimple_cmd.cpp
    cimple_diagnostics.cpp
    cimple_ninja.cpp
    cimple_pkg_config.cpp
    cimple_build_pkg.cpp
    cimple_requests.cpp
    cimple_tar.cpp
    cimple_oci.cpp
)

add_executable(cimple-bin
    cimple.cpp
)

find_package(cps REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(boost REQUIRED COMPONENTS program_options)
find_package(lua REQUIRED)
find_package(curl REQUIRED)
find_package(LibArchive REQUIRED)
find_package(cpp-subprocess REQUIRED)
find_package(uuid_v4 REQUIRED)

target_link_libraries(cimple PRIVATE
    cps
    tomlplusplus::tomlplusplus
    CURL::libcurl
    LibArchive::LibArchive
    cpp-subprocess::subprocess
    uuid_v4::uuid_v4
)

if(MSVC)
    target_compile_options(cimple PUBLIC /arch:SSE4.1 /arch:AVX2)
else()
    target_compile_options(cimple PUBLIC -msse4.1 -mavx2)
endif()

if(TARGET CURL::libcurl AND WIN32)
    target_link_libraries(CURL::libcurl INTERFACE bcrypt)
endif()

target_link_libraries(cimple-bin PRIVATE
    Boost::program_options
    cimple
)

set_target_properties(cimple-bin PROPERTIES OUTPUT_NAME "cimple")
