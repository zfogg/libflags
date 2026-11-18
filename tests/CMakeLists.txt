if(NOT LIBFLAGS_BUILD_TESTS)
  return()
endif()

enable_testing()

include(Catch)

add_executable(libflags-test parsing/flags_test.c catch2_tests.cpp)
target_link_libraries(libflags-test PRIVATE libflags Catch2::Catch2WithMain)
catch_discover_tests(libflags-test)

add_executable(libflags-json-test json/flags_json_test.c catch2_tests.cpp)
target_compile_definitions(libflags-json-test PRIVATE LIBFLAGS_JSON_CONTRACT_TEST)
target_link_libraries(libflags-json-test PRIVATE libflags Catch2::Catch2WithMain)
catch_discover_tests(libflags-json-test)
