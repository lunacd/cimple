add_library(libcimple
    cimple_config.cpp
    cimple_util.cpp
    cimple_build.cpp
    cimple_cmd.cpp
    cimple_diagnostics.cpp
    cimple_ninja.cpp
)

add_executable(cimple
    cimple.cpp
)

find_package(cps REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(boost REQUIRED COMPONENTS process)

target_link_libraries(libcimple PRIVATE
    cps
    tomlplusplus::tomlplusplus
    Boost::process
)

target_link_libraries(cimple PRIVATE libcimple)

target_compile_definitions(libcimple PRIVATE BOOST_PROCESS_USE_STD_FS)
