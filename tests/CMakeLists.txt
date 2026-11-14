if(NOT LIBFLAGS_BUILD_TESTS)
  return()
endif()

enable_testing()

add_executable(libflags-test parsing/flags_test.c)
target_link_libraries(libflags-test PRIVATE flags)
add_test(NAME libflags-test COMMAND libflags-test)

if(LIBFLAGS_YYJSON_LIBRARIES)
  add_executable(libflags-json-test json/flags_json_test.c)
  target_link_libraries(libflags-json-test PRIVATE flags)
  add_test(NAME libflags-json-test COMMAND libflags-json-test)
endif()
