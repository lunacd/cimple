add_library(libcimple
    cimple_config.cpp
    cimple_util.cpp
    cimple_build.cpp
    cimple_cmd.cpp
    cimple_diagnostics.cpp
)

add_executable(cimple
    cimple.cpp
)

find_package(cps REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(graaf REQUIRED)

target_link_libraries(libcimple PRIVATE
    cps
    tomlplusplus::tomlplusplus
    Graaf::Graaf
)

target_link_libraries(cimple PRIVATE libcimple)
