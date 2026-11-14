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
/**
 * @brief Emits a formatted diagnostic event when logging is enabled.
 * @param context Context emitting the event.
 * @param level Event severity.
 * @param category Event category.
 * @param option_name Related option name, or NULL.
 * @param format printf-style format string.
 * @note Additional arguments must match @p format.
 */
void flags_log(flags_context_t *context, flags_log_level_t level, flags_log_category_t category, const char *option_name, const char *format, ...);
/**
 * @brief Emits a secret-aware value diagnostic for an option.
 * @param context Context emitting the event.
 * @param level Event severity.
 * @param category Event category.
 * @param option_name Option name.
 * @param value Value text.
 */
void flags_log_value(flags_context_t *context, flags_log_level_t level, flags_log_category_t category, const char *option_name, const char *value);
