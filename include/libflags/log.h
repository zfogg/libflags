/** @file
 * @brief Per-context diagnostics and logging hooks.
 */
#pragma once

#include <libflags/flags.h>

/** Installs or clears a context logging hook. */
bool flags_set_logging(flags_context_t *, const flags_logging_config_t *);
/** Emits a formatted diagnostic event when logging is enabled. */
void flags_log(flags_context_t *, flags_log_level_t, flags_log_category_t, const char *, const char *, ...);
/** Emits a secret-aware value diagnostic for an option. */
void flags_log_value(flags_context_t *, flags_log_level_t, flags_log_category_t, const char *, const char *);
