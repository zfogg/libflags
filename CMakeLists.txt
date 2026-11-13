cmake_minimum_required(VERSION 3.20)
if(NOT DEFINED CMAKE_C_COMPILER)
  set(CMAKE_C_COMPILER clang CACHE FILEPATH "C compiler")
endif()
project(libflags VERSION 0.1.0 LANGUAGES C)
include(GNUInstallDirs)
include(FetchContent)

set(LIBFLAGS_DEPS_CACHE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.deps-cache" CACHE PATH "Shared source cache for libflags dependencies")
set(LIBFLAGS_LIBPLATFORM_SOURCE_DIR "" CACHE PATH "Existing libplatform source tree")
set(LIBFLAGS_PCRE2_INCLUDE_DIRS "" CACHE STRING "PCRE2 include directories supplied by the embedding project")
set(LIBFLAGS_PCRE2_LIBRARIES "" CACHE STRING "PCRE2 libraries or targets supplied by the embedding project")
set(LIBFLAGS_YYJSON_INCLUDE_DIRS "" CACHE STRING "yyjson include directories supplied by the embedding project")
set(LIBFLAGS_YYJSON_LIBRARIES "" CACHE STRING "yyjson libraries or targets supplied by the embedding project")
if(NOT TARGET libplatform::libplatform AND NOT TARGET libplatform AND NOT TARGET libplatform-objects)
  # Embedders may add libplatform before libflags. Reuse that target instead of
  # fetching a second copy into the dependency cache.
  if(TARGET libplatform)
    add_library(libplatform::libplatform ALIAS libplatform)
  endif()
endif()
if(NOT TARGET libplatform::libplatform AND NOT TARGET libplatform AND NOT TARGET libplatform-objects)
  if(LIBFLAGS_LIBPLATFORM_SOURCE_DIR AND EXISTS "${LIBFLAGS_LIBPLATFORM_SOURCE_DIR}/CMakeLists.txt")
    set(LIBPLATFORM_BUILD_TESTS OFF CACHE BOOL "Disable libplatform standalone tests" FORCE)
    add_subdirectory("${LIBFLAGS_LIBPLATFORM_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/libplatform")
  else()
    set(LIBPLATFORM_BUILD_TESTS OFF CACHE BOOL "Disable libplatform standalone tests" FORCE)
    FetchContent_Declare(libplatform
      GIT_REPOSITORY https://github.com/zfogg/libplatform.git
      GIT_TAG d09bdff
      SOURCE_DIR "${LIBFLAGS_DEPS_CACHE_ROOT}/libplatform-src")
    FetchContent_MakeAvailable(libplatform)
  endif()
endif()
set(LIBFLAGS_TOMLC17_SOURCE_DIR "" CACHE PATH "Existing tomlc17 source tree")
if(LIBFLAGS_TOMLC17_SOURCE_DIR AND EXISTS "${LIBFLAGS_TOMLC17_SOURCE_DIR}/src/tomlc17.c")
  add_library(libflags-tomlc17 STATIC "${LIBFLAGS_TOMLC17_SOURCE_DIR}/src/tomlc17.c")
  target_include_directories(libflags-tomlc17 PUBLIC "${LIBFLAGS_TOMLC17_SOURCE_DIR}/src")
else()
  FetchContent_Declare(tomlc17
    GIT_REPOSITORY https://github.com/cktan/tomlc17.git
    GIT_TAG master
    SOURCE_DIR "${LIBFLAGS_DEPS_CACHE_ROOT}/tomlc17-src")
  FetchContent_MakeAvailable(tomlc17)
  if(NOT TARGET libflags-tomlc17)
    add_library(libflags-tomlc17 STATIC "${tomlc17_SOURCE_DIR}/src/tomlc17.c")
    target_include_directories(libflags-tomlc17 PUBLIC "${tomlc17_SOURCE_DIR}/src")
  endif()
endif()
add_library(flags
  lib/options.c
  lib/json.c
  lib/log.c
  lib/validation.c
  lib/util/ip.c
  lib/util/url.c
  lib/commands.c
  lib/parsing/parsers.c
  lib/builder/builder.c
  lib/config/config.c
  lib/completions/completions.c
  lib/manpage/manpage.c
  lib/manpage/formatter.c)
add_library(libflags::flags ALIAS flags)
target_include_directories(flags PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(flags PUBLIC c_std_11)
target_link_libraries(flags PUBLIC libplatform::libplatform libflags-tomlc17)
if(WIN32)
  target_link_libraries(flags PUBLIC ws2_32)
endif()
if(LIBFLAGS_PCRE2_INCLUDE_DIRS)
  target_include_directories(flags PRIVATE ${LIBFLAGS_PCRE2_INCLUDE_DIRS})
  target_compile_definitions(flags PUBLIC LIBFLAGS_HAS_PCRE2=1 PCRE2_STATIC=1)
endif()
if(LIBFLAGS_PCRE2_LIBRARIES)
  target_link_libraries(flags PUBLIC ${LIBFLAGS_PCRE2_LIBRARIES})
endif()
if(LIBFLAGS_YYJSON_INCLUDE_DIRS)
  target_include_directories(flags PRIVATE ${LIBFLAGS_YYJSON_INCLUDE_DIRS})
  target_compile_definitions(flags PRIVATE LIBFLAGS_HAS_YYJSON=1)
endif()
if(LIBFLAGS_YYJSON_LIBRARIES)
  target_link_libraries(flags PUBLIC ${LIBFLAGS_YYJSON_LIBRARIES})
endif()
if(LIBFLAGS_BUILD_TESTS)
  enable_testing()
  add_executable(libflags-test tests/parsing/flags_test.c)
  target_link_libraries(libflags-test PRIVATE flags)
  add_test(NAME libflags-test COMMAND libflags-test)
  if(LIBFLAGS_YYJSON_LIBRARIES)
    add_executable(libflags-json-test tests/json/flags_json_test.c)
    target_link_libraries(libflags-json-test PRIVATE flags)
    add_test(NAME libflags-json-test COMMAND libflags-json-test)
  endif()
endif()
install(TARGETS flags EXPORT libflagsTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

