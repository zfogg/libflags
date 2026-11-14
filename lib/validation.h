#pragma once

#include <stdbool.h>
#include <stdint.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

/* Regex validation is an implementation detail of option validation. */
static inline bool libflags_validate_regex(const char *text, const char *pattern, uint32_t options) {
  int error_code;
  PCRE2_SIZE error_offset;
  pcre2_code *code;
  pcre2_match_data *match_data;
  int match;
  if (!text || !pattern)
    return false;
  code = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, options, &error_code, &error_offset, NULL);
  if (!code)
    return false;
  match_data = pcre2_match_data_create_from_pattern(code, NULL);
  if (!match_data) {
    pcre2_code_free(code);
    return false;
  }
  match = pcre2_match(code, (PCRE2_SPTR)text, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);
  pcre2_match_data_free(match_data);
  pcre2_code_free(code);
  return match >= 0;
}
