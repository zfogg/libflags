/** @file
 * @brief Parsers for common semantic option values.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <libflags/flags.h>

/** An owned list of string values parsed from comma-separated text. */
typedef struct flags_string_list {
  /** Allocated item strings. */
  char **items;
  /** Number of items. */
  size_t count;
} flags_string_list_t;

/** An owned list of signed integer values parsed from comma-separated text. */
typedef struct flags_int_list {
  /** Allocated integer values. */
  int64_t *items;
  /** Number of items. */
  size_t count;
} flags_int_list_t;

/** Parses true/false, yes/no, on/off, and 1/0 spellings. */
bool flags_parse_bool_relaxed(const char *, bool *);
/** Parses a duration such as `500ms`, `5s`, `2m`, or `1h` to nanoseconds. */
bool flags_parse_duration(const char *, uint64_t *);
/** Parses a byte size such as `4KiB`, `2MiB`, or `1GB`. */
bool flags_parse_size(const char *, uint64_t *);
/** Parses one spelling from a caller-owned enum table. */
bool flags_parse_enum(const char *, const char *const *, size_t, size_t *);
/** Parses a numeric decibel value with an optional `dB` suffix or `auto`. */
bool flags_parse_decibel_or_auto(const char *, flags_decibel_t *);
/** Parses a percentage or `auto`, preserving whether it was automatic. */
bool flags_parse_percent_or_auto(const char *, flags_percent_t *);
/** Parses a non-empty comma-separated string list. */
bool flags_parse_string_list(const char *, flags_string_list_t *);
/** Parses a non-empty comma-separated signed integer list. */
bool flags_parse_int_list(const char *, flags_int_list_t *);
/** Releases a string list and its items. */
void flags_free_string_list(flags_string_list_t *);
/** Releases an integer list. */
void flags_free_int_list(flags_int_list_t *);
