target_sources(libcimple PUBLIC FILE_SET HEADERS FILES
    cimple_build.hpp
    cimple_config.hpp
    cimple_util.hpp
    cimple_cmd.hpp
    cimple_diagnostics.hpp
)

target_include_directories(libcimple PUBLIC .)
