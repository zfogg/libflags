/** @file
 * @brief Generic textual value parser declarations.
 */
#pragma once

#include <libflags/values.h>

/** @brief Parses true/false, yes/no, on/off, and 1/0 spellings. */
bool flags_parse_bool_relaxed(const char *text, bool *value);
/** @brief Parses a duration to nanoseconds. */
bool flags_parse_duration(const char *text, uint64_t *nanoseconds);
/** @brief Parses a byte size. */
bool flags_parse_size(const char *text, uint64_t *bytes);
/** @brief Parses one spelling from an enum table. */
bool flags_parse_enum(const char *text, const char *const *values, size_t value_count, size_t *index);
/** @brief Parses a decibel value or `auto`. */
bool flags_parse_decibel_or_auto(const char *text, flags_decibel_t *value);
/** @brief Parses a percentage or `auto`. */
bool flags_parse_percent_or_auto(const char *text, flags_percent_t *value);
/** @brief Parses a non-empty comma-separated string list. */
bool flags_parse_string_list(const char *text, flags_string_list_t *value);
/** @brief Parses a non-empty comma-separated signed integer list. */
bool flags_parse_int_list(const char *text, flags_int_list_t *value);
/** @brief Releases a string list and its items. */
void flags_free_string_list(flags_string_list_t *value);
/** @brief Releases an integer list. */
void flags_free_int_list(flags_int_list_t *value);
