/** @file
 * @brief Parsers for common semantic option values.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <libflags/flags.h>

/**
 * @brief Owned list of string values parsed from comma-separated text.
 */
typedef struct flags_string_list {
  /** Allocated item strings. */
  char **items;
  /** Number of items. */
  size_t count;
} flags_string_list_t;

/**
 * @brief Owned list of signed integer values parsed from comma-separated text.
 */
typedef struct flags_int_list {
  /** Allocated integer values. */
  int64_t *items;
  /** Number of items. */
  size_t count;
} flags_int_list_t;

/**
 * @brief Parses true/false, yes/no, on/off, and 1/0 spellings.
 * @param text Input spelling.
 * @param value Destination value.
 * @return Whether parsing succeeded.
 */
bool flags_parse_bool_relaxed(const char *text, bool *value);
/**
 * @brief Parses a duration to nanoseconds.
 * @param text Input duration.
 * @param nanoseconds Destination value.
 * @return Whether parsing succeeded.
 */
bool flags_parse_duration(const char *text, uint64_t *nanoseconds);
/**
 * @brief Parses a byte size.
 * @param text Input size.
 * @param bytes Destination value.
 * @return Whether parsing succeeded.
 */
bool flags_parse_size(const char *text, uint64_t *bytes);
/**
 * @brief Parses one spelling from an enum table.
 * @param text Input spelling.
 * @param values Enum spellings.
 * @param value_count Number of spellings.
 * @param index Destination index.
 * @return Whether parsing succeeded.
 */
bool flags_parse_enum(const char *text, const char *const *values, size_t value_count, size_t *index);
/**
 * @brief Parses a decibel value or `auto`.
 * @param text Input value.
 * @param value Destination value.
 * @return Whether parsing succeeded.
 */
bool flags_parse_decibel_or_auto(const char *text, flags_decibel_t *value);
/**
 * @brief Parses a percentage or `auto`.
 * @param text Input value.
 * @param value Destination value.
 * @return Whether parsing succeeded.
 */
bool flags_parse_percent_or_auto(const char *text, flags_percent_t *value);
/**
 * @brief Parses a non-empty comma-separated string list.
 * @param text Input list.
 * @param value Destination list.
 * @return Whether parsing succeeded.
 * @note The destination owns allocated items after success.
 */
bool flags_parse_string_list(const char *text, flags_string_list_t *value);
/**
 * @brief Parses a non-empty comma-separated signed integer list.
 * @param text Input list.
 * @param value Destination list.
 * @return Whether parsing succeeded.
 */
bool flags_parse_int_list(const char *text, flags_int_list_t *value);
/**
 * @brief Releases a string list and its items.
 * @param value List to release.
 */
void flags_free_string_list(flags_string_list_t *value);
/**
 * @brief Releases an integer list.
 * @param value List to release.
 */
void flags_free_int_list(flags_int_list_t *value);
