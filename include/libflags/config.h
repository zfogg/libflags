/** @file
 * @brief Generic TOML configuration import/export for a flags context.
 */
#pragma once
#include <libflags/flags.h>

/**
 * @brief Imports matching dotted TOML keys with configuration precedence.
 * @param context Context whose schema and storage receive the values.
 * @param path TOML path, or `-` for standard input.
 * @return `true` when the document is valid and applied.
 */
bool flags_config_load_toml(flags_context_t *context, const char *path);
/**
 * @brief Writes the currently bound scalar values as a TOML document.
 * @param context Context whose values are serialized.
 * @param path Output path, or `-` for standard output.
 * @return `true` when the document was written successfully.
 */
bool flags_config_write_toml(const flags_context_t *context, const char *path);
