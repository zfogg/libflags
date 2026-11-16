/** @file
 * @brief Generic primitive option storage helpers.
 */
#pragma once

#include <libflags/flags.h>

/**
 * @brief Tests whether a bound primitive differs from its schema default.
 * @param type Primitive value type.
 * @param field Bound storage.
 * @param field_size Bound storage size.
 * @param default_value Optional default storage.
 * @param default_size Default storage size.
 * @return `true` when the value is explicitly different from its default.
 */
bool flags_storage_is_set(flags_value_type_t type, const void *field, size_t field_size, const void *default_value, size_t default_size);

/**
 * @brief Parses a primitive spelling directly into bound storage.
 * @param type Primitive value type.
 * @param field Destination storage.
 * @param field_size Destination size.
 * @param text Input spelling, or NULL to toggle a boolean.
 * @return `true` when the spelling was accepted and stored.
 */
bool flags_storage_apply_text(flags_value_type_t type, void *field, size_t field_size, const char *text);
