/** @file
 * @brief Generic configuration-key normalization helpers.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Builds a normalized dotted TOML key.
 * @param group Group name; converted to lowercase.
 * @param field_name Field name; dashes become underscores and the result is lowercased.
 * @param buffer Destination buffer.
 * @param buffer_size Destination capacity in bytes.
 * @return `true` when the key fits and was written.
 */
bool flags_config_make_toml_key(const char *group, const char *field_name, char *buffer, size_t buffer_size);

/**
 * @brief Builds a long command-line spelling from an option name.
 * @param name Canonical option name.
 * @param buffer Destination buffer.
 * @param buffer_size Destination capacity in bytes.
 * @return `true` when the spelling fits and was written.
 */
bool flags_config_make_cli_flag(const char *name, char *buffer, size_t buffer_size);
