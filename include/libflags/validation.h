#pragma once
#include <libflags/flags.h>

/** Matches @p text against a PCRE2 pattern using the 8-bit library. */
bool flags_validate_regex(const char *text, const char *pattern, uint32_t options);
