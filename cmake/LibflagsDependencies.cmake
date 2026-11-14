include(FetchContent)

function(libflags_configure_pcre2)
  # Preserve fetched PCRE2 as a logical CMake target; its output filename is
  # generator- and configuration-dependent.
  if(TARGET pcre2-8-static)
    set(LIBFLAGS_PCRE2_TARGET pcre2-8-static PARENT_SCOPE)
  elseif(TARGET pcre2-8)
    set(LIBFLAGS_PCRE2_TARGET pcre2-8 PARENT_SCOPE)
  endif()

  if(LIBFLAGS_PCRE2_INCLUDE_DIRS AND LIBFLAGS_PCRE2_LIBRARIES
      AND NOT (LIBFLAGS_PCRE2_LIBRARIES MATCHES "^pcre2-8(-static)?$"
               AND NOT LIBFLAGS_PCRE2_TARGET))
    set(_libflags_pcre2_headers_valid FALSE)
    foreach(_include_dir IN LISTS LIBFLAGS_PCRE2_INCLUDE_DIRS)
      if(EXISTS "${_include_dir}/pcre2.h")
        set(_libflags_pcre2_headers_valid TRUE)
      endif()
    endforeach()
    if(_libflags_pcre2_headers_valid)
      unset(_libflags_pcre2_headers_valid)
      return()
    endif()
    unset(_libflags_pcre2_headers_valid)
    unset(LIBFLAGS_PCRE2_INCLUDE_DIRS CACHE)
    unset(LIBFLAGS_PCRE2_LIBRARIES CACHE)
  endif()

  find_package(PkgConfig QUIET)
  if(PkgConfig_FOUND)
    pkg_check_modules(LIBFLAGS_SYSTEM_PCRE2 QUIET libpcre2-8)
  endif()

  if(LIBFLAGS_SYSTEM_PCRE2_FOUND)
    set(LIBFLAGS_PCRE2_INCLUDE_DIRS "${LIBFLAGS_SYSTEM_PCRE2_INCLUDE_DIRS}" CACHE STRING
        "PCRE2 include directories supplied to libflags" FORCE)
    set(LIBFLAGS_PCRE2_LIBRARIES "${LIBFLAGS_SYSTEM_PCRE2_LINK_LIBRARIES}" CACHE STRING
        "PCRE2 libraries supplied to libflags" FORCE)
    if(LIBFLAGS_SYSTEM_PCRE2_LIBRARY_DIRS)
      set(LIBFLAGS_PCRE2_LIBRARY_DIRS "${LIBFLAGS_SYSTEM_PCRE2_LIBRARY_DIRS}" CACHE STRING
          "PCRE2 library directories supplied to libflags" FORCE)
    endif()
    return()
  endif()

  set(PCRE2_BUILD_TESTS OFF CACHE BOOL "Disable PCRE2 tests in libflags" FORCE)
  set(PCRE2_BUILD_PCRE2_8 ON CACHE BOOL "Build PCRE2 8-bit support" FORCE)
  set(PCRE2_BUILD_PCRE2_16 OFF CACHE BOOL "Disable PCRE2 16-bit support" FORCE)
  set(PCRE2_BUILD_PCRE2_32 OFF CACHE BOOL "Disable PCRE2 32-bit support" FORCE)
  set(PCRE2_SUPPORT_JIT ON CACHE BOOL "Build PCRE2 JIT support" FORCE)
  FetchContent_Declare(libflags_pcre2
    GIT_REPOSITORY https://github.com/PCRE2Project/pcre2.git
    GIT_TAG pcre2-10.44
    SOURCE_DIR "${LIBFLAGS_DEPS_CACHE_ROOT}/pcre2-src")
  FetchContent_MakeAvailable(libflags_pcre2)
  if(TARGET pcre2-8-static)
    set(LIBFLAGS_PCRE2_TARGET pcre2-8-static PARENT_SCOPE)
  elseif(TARGET pcre2-8)
    set(LIBFLAGS_PCRE2_TARGET pcre2-8 PARENT_SCOPE)
  else()
    message(FATAL_ERROR "PCRE2 was fetched, but no 8-bit PCRE2 target was created")
  endif()
  set(LIBFLAGS_PCRE2_INCLUDE_DIRS "${libflags_pcre2_SOURCE_DIR}/src;${libflags_pcre2_BINARY_DIR}" CACHE STRING
      "PCRE2 include directory supplied to libflags" FORCE)
endfunction()
