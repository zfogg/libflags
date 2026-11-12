#include <libflags/log.h>

#include "internal/context.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static size_t log_find(const flags_schema_t *schema, const char *name) {
  for (size_t i = 0; i < schema->option_count; ++i) {
    const flags_option_spec_t *option = &schema->options[i];
    if (strcmp(option->name, name) == 0)
      return i;
    for (size_t alias = 0; alias < option->alias_count; ++alias)
      if (option->aliases[alias] && strcmp(option->aliases[alias], name) == 0)
        return i;
  }
  return SIZE_MAX;
}

bool flags_set_logging(flags_context_t *context, const flags_logging_config_t *config) {
  if (!context)
    return false;
  context->logging = config ? *config : (flags_logging_config_t){0};
  return true;
}

void flags_log(flags_context_t *context, flags_log_level_t level, flags_log_category_t category, const char *option_name, const char *format, ...) {
  if (!context || !context->logging.callback || level < context->logging.minimum_level)
    return;
  char message[1024];
  va_list arguments;
  va_start(arguments, format);
  vsnprintf(message, sizeof(message), format, arguments);
  va_end(arguments);
  flags_log_event_t event = {.level = level, .category = category, .message = message, .option_name = option_name, .revision = context->revision};
  context->logging.callback(&event, context->logging.user_data);
}

void flags_log_value(flags_context_t *context, flags_log_level_t level, flags_log_category_t category, const char *option_name, const char *value) {
  if (!context || !option_name)
    return;
  size_t index = log_find(context->schema, option_name);
  if (index == SIZE_MAX)
    return;
  flags_log(context, level, category, context->schema->options[index].name, "value=%s", context->schema->options[index].secret ? "<redacted>" : (value ? value : "<null>"));
}
