#include <catch2/catch_test_macros.hpp>

extern "C" int libflags_json_contract(void);
extern "C" int libflags_parsing_contract(void);

#ifdef LIBFLAGS_JSON_CONTRACT_TEST
TEST_CASE("libflags JSON contract", "[json]") {
  REQUIRE(libflags_json_contract() == 0);
}
#else
TEST_CASE("libflags parsing contract", "[parsing]") {
  REQUIRE(libflags_parsing_contract() == 0);
}
#endif
