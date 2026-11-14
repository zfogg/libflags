/** @file
 * @brief Per-context diagnostics and logging hooks.
 */
#pragma once

#include <libflags/flags.h>

/**
 * @brief Installs or clears a context logging hook.
 * @param context Context to configure.
 * @param config Logging configuration, or NULL to clear it.
 * @return Whether configuration succeeded.
 */
bool flags_set_logging(flags_context_t *context, const flags_logging_config_t *config);
