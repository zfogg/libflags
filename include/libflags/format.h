/**
 * @file format.h
 * @brief Generic option display formatting.
 */
#pragma once

#include <libflags/flags.h>
#include <stddef.h>

/**
 * @brief Returns the conventional help placeholder for a value type.
 * @param type Value type.
 * @return Static placeholder text, or an empty string for non-value actions.
 */
const char *flags_type_placeholder(flags_value_type_t type);

/**
 * @brief Formats a scalar default value for help output.
 * @param type Value type represented by @p default_value.
 * @param default_value Typed default value.
 * @param buffer Destination buffer.
 * @param buffer_size Destination capacity.
 * @return Number of characters that would have been written, or zero for unsupported input.
 */
int flags_format_default_value(flags_value_type_t type, const void *default_value, char *buffer, size_t buffer_size);
