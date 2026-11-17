#include <libflags/help.h>
#include <libflags/format.h>
#include <libplatform/system.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static const char *flags_help_placeholder(const flags_option_spec_t *option) {
  if (option->metavar)
    return option->metavar;
  switch (option->type) {
  case FLAGS_TYPE_BOOL:
    return NULL;
  case FLAGS_TYPE_INT64:
  case FLAGS_TYPE_UINT64:
  case FLAGS_TYPE_PORT:
    return "INTEGER";
  case FLAGS_TYPE_DOUBLE:
    return "NUMBER";
  default:
    return "VALUE";
  }
}

static size_t flags_help_option_width(const flags_schema_t *schema) {
  size_t width = 0;
  for (size_t i = 0; i < schema->option_count; ++i) {
    const flags_option_spec_t *option = &schema->options[i];
    size_t current = strlen(option->name) + 2;
    if (option->short_name)
      current += 4;
    const char *placeholder = flags_help_placeholder(option);
    if (placeholder)
      current += strlen(placeholder) + 1;
    if (current > width)
      width = current;
  }
  return width;
}

static const char *flags_help_default(const flags_option_spec_t *option, char *buffer, size_t buffer_size) {
  if (!option->default_value || !buffer || buffer_size == 0)
    return NULL;
  return flags_format_default_value(option->type, option->default_value, buffer, buffer_size) > 0 ? buffer : NULL;
}

bool flags_help_write_options(const flags_schema_t *schema, const char *program_name, const char *description, FILE *output) {
  if (!schema || !program_name || !output)
    return false;
  fprintf(output, "%s\n", program_name);
  if (description && *description)
    fprintf(output, "%s\n", description);
  fputc('\n', output);
  const size_t width = flags_help_option_width(schema);
  for (size_t i = 0; i < schema->option_count; ++i) {
    const flags_option_spec_t *option = &schema->options[i];
    char spelling[256];
    int written = option->short_name ? snprintf(spelling, sizeof(spelling), "-%c, --%s", option->short_name, option->name) : snprintf(spelling, sizeof(spelling), "--%s", option->name);
    if (written < 0 || (size_t)written >= sizeof(spelling))
      return false;
    const char *placeholder = flags_help_placeholder(option);
    if (placeholder && (size_t)written + strlen(placeholder) + 1 < sizeof(spelling)) {
      spelling[written++] = ' ';
      platform_strcpy(spelling + written, sizeof(spelling) - (size_t)written, placeholder);
    }
    char default_value[128];
    const char *default_text = flags_help_default(option, default_value, sizeof(default_value));
    fprintf(output, "  %-*s  %s%s\n", (int)width, spelling, option->description ? option->description : "Option value.", option->required ? " (required)" : "");
    if (default_text)
      fprintf(output, "  %*s  (default: %s)\n", (int)width, "", default_text);
  }
  return !ferror(output);
}
