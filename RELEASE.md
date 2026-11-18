# libflags release package

Extract this archive anywhere; it has no required installation directory. The
`static/` and `shared/` trees contain the corresponding libraries, headers,
CMake package, pkg-config file, and licenses.

For CMake, set `CMAKE_PREFIX_PATH` to the extracted `static` or `shared`
directory and use `find_package(libflags CONFIG REQUIRED)`, then link
`libflags::flags`. For pkg-config, set `PKG_CONFIG_PATH` to that directory's
`lib/pkgconfig` folder and request `libflags`.

Static consumers must also link the platform and bundled dependency libraries
listed by their toolchain; shared consumers need the matching runtime library
directory on their platform's loader path. Use the same compiler/runtime family
used to build the archive (MSVC-compatible Clang/ MSVC libraries are not
link-compatible with MinGW GCC libraries).
