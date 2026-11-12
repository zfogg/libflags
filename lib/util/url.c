#include <libflags/util/url.h>

#include <string.h>

#ifdef LIBFLAGS_HAS_PCRE2
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#endif

static const char *url_scheme_end(const char *url) {
  const char *separator;
  if (!url)
    return NULL;
  separator = strstr(url, "://");
  return separator ? separator : NULL;
}

bool flags_url_is_valid(const char *url) {
#ifdef LIBFLAGS_HAS_PCRE2
  static const char pattern[] = "^(https?|wss?|tcp)://[^/?#[:space:]]+(?:/[^[:space:]]*)?(?:\\?[^[:space:]]*)?(?:#[^[:space:]]*)?$";
  int error_code;
  PCRE2_SIZE error_offset;
  pcre2_code *code;
  pcre2_match_data *match_data;
  int result;
  if (!url || !*url)
    return false;
  code = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, &error_code, &error_offset, NULL);
  if (!code)
    return false;
  match_data = pcre2_match_data_create_from_pattern(code, NULL);
  if (!match_data) {
    pcre2_code_free(code);
    return false;
  }
  result = pcre2_match(code, (PCRE2_SPTR)url, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);
  pcre2_match_data_free(match_data);
  pcre2_code_free(code);
  return result >= 0;
#else
  (void)url;
  return false;
#endif
}

bool flags_url_is_websocket_scheme(const char *scheme) {
  return scheme && (strcmp(scheme, "ws") == 0 || strcmp(scheme, "wss") == 0);
}

bool flags_url_is_websocket(const char *url) {
  const char *end = url_scheme_end(url);
  size_t length = end ? (size_t)(end - url) : 0;
  return length == 2 && (strncmp(url, "ws", 2) == 0 || strncmp(url, "WS", 2) == 0) && flags_url_is_valid(url);
}

bool flags_url_is_tcp_scheme(const char *scheme) {
  return scheme && strcmp(scheme, "tcp") == 0;
}

bool flags_url_is_tcp(const char *url) {
  const char *end = url_scheme_end(url);
  return end && (size_t)(end - url) == 3 && strncmp(url, "tcp", 3) == 0 && flags_url_is_valid(url);
}
