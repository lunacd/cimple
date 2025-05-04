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

target_link_libraries(cimple PRIVATE
    cps
    tomlplusplus::tomlplusplus
    CURL::libcurl
    LibArchive::LibArchive
    cpp-subprocess::subprocess
)

if(TARGET CURL::libcurl AND WIN32)
    target_link_libraries(CURL::libcurl INTERFACE bcrypt)
endif()

target_link_libraries(cimple-bin PRIVATE
    Boost::program_options
    cimple
)

set_target_properties(cimple-bin PROPERTIES OUTPUT_NAME "cimple")
