if(NOT LIBFLAGS_BUILD_TESTS)
  return()
endif()

enable_testing()

include(Catch)

# Catch2 discovers tests by executing the finished binaries during a
# multi-config build.  When BUILD_SHARED_LIBS is enabled, Windows does not
# automatically search the sibling dependency output directories for the DLLs
# loaded by libflags and libplatform.  Pass those target-owned directories to
# Catch2 so discovery and ctest use the same runtime search path as the build.
set(LIBFLAGS_TEST_DLL_PATHS
  "$<TARGET_FILE_DIR:libflags>"
  "$<TARGET_FILE_DIR:libplatform>"
  "$<TARGET_FILE_DIR:libflags-tomlc17>"
  "$<TARGET_FILE_DIR:utf8proc>"
  "$<TARGET_FILE_DIR:yyjson>"
  "$<TARGET_FILE_DIR:Catch2WithMain>")

add_executable(libflags-test parsing/flags_test.c catch2_tests.cpp)
target_link_libraries(libflags-test PRIVATE libflags Catch2::Catch2WithMain)
catch_discover_tests(libflags-test DL_PATHS ${LIBFLAGS_TEST_DLL_PATHS})

add_executable(libflags-json-test json/flags_json_test.c catch2_tests.cpp)
target_compile_definitions(libflags-json-test PRIVATE LIBFLAGS_JSON_CONTRACT_TEST)
target_link_libraries(libflags-json-test PRIVATE libflags Catch2::Catch2WithMain)
catch_discover_tests(libflags-json-test DL_PATHS ${LIBFLAGS_TEST_DLL_PATHS})
