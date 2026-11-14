/** @file
 * @brief Per-context diagnostics and logging hooks.
 */
#pragma once

#include <libflags/flags.h>

/** Installs or clears a context logging hook. */
bool flags_set_logging(flags_context_t *context, const flags_logging_config_t *config);
/** Emits a formatted diagnostic event when logging is enabled. */
void flags_log(flags_context_t *context, flags_log_level_t level, flags_log_category_t category, const char *option_name, const char *format, ...);
/** Emits a secret-aware value diagnostic for an option. */
void flags_log_value(flags_context_t *context, flags_log_level_t level, flags_log_category_t category, const char *option_name, const char *value);
