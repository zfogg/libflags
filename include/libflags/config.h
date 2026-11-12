/** @file
 * @brief Generic TOML configuration import/export for a flags context.
 */
#pragma once
#include <libflags/flags.h>

/** Imports matching dotted TOML keys with configuration precedence. */
bool flags_config_load_toml(flags_context_t *, const char *path);
/** Writes the currently bound scalar values as a TOML document. */
bool flags_config_write_toml(const flags_context_t *, const char *path);
