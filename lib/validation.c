#include <libflags/validation.h>

#include <stddef.h>

#ifdef LIBFLAGS_HAS_PCRE2
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#endif

bool flags_validate_regex(const char *text, const char *pattern, uint32_t options) {
#ifdef LIBFLAGS_HAS_PCRE2
  int error_code;
  PCRE2_SIZE error_offset;
  pcre2_code *code;
  int match;
  if (!text || !pattern)
    return false;
  code = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, options, &error_code, &error_offset, NULL);
  if (!code)
    return false;
  match = pcre2_match(code, (PCRE2_SPTR)text, PCRE2_ZERO_TERMINATED, 0, 0, NULL, NULL);
  pcre2_code_free(code);
  return match >= 0;
#else
  (void)text;
  (void)pattern;
  (void)options;
  return false;
#endif
}
