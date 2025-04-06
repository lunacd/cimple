add_library(libcimple
    cimple_config.cpp
    cimple_util.cpp
    cimple_build.cpp
    cimple_cmd.cpp
    cimple_diagnostics.cpp
    cimple_ninja.cpp
    cimple_pkg_config.cpp
    cimple_build_pkg.cpp
)

add_executable(cimple
    cimple.cpp
)

find_package(cps REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(boost REQUIRED COMPONENTS process program_options)
find_package(lua REQUIRED)

target_link_libraries(libcimple PRIVATE
    cps
    tomlplusplus::tomlplusplus
    Boost::process
    lua::lua
)

if (WIN32)
    target_link_libraries(libcimple PRIVATE wsock32 bcrypt)
endif()

target_link_libraries(cimple PRIVATE
    Boost::program_options
)

target_link_libraries(cimple PRIVATE libcimple)

target_compile_definitions(libcimple PRIVATE BOOST_PROCESS_USE_STD_FS)
