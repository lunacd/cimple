target_sources(libcimple PUBLIC FILE_SET HEADERS FILES
    cimple_build.hpp
    cimple_config.hpp
    cimple_util.hpp
    cimple_cmd.hpp
    cimple_diagnostics.hpp
    cimple_ninja.hpp
    cimple_pkg_config.hpp
    cimple_build_pkg.hpp
    cimple_requests.hpp
    cimple_tar.hpp
)

target_include_directories(libcimple PUBLIC .)
