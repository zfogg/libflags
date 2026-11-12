/** @file
 * @brief Parsers for common semantic option values.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** A decibel value that can represent automatic gain independently of zero. */
typedef struct flags_decibel {
  double value; /**< Numeric decibel value. */
  bool automatic; /**< Whether the value is `auto`. */
} flags_decibel_t;

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

