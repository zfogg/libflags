#include <libflags/config.h>
#include <tomlc17.h>

#include <stdio.h>
#include <string.h>

static bool flags_config_write_string(FILE *output, const char *name, const char *value) {
  if (fprintf(output, "%s = \"", name) < 0)
    return false;
  for (const unsigned char *cursor = (const unsigned char *)(value ? value : ""); *cursor; ++cursor) {
    switch (*cursor) {
    case '\\':
      if (fputs("\\\\", output) == EOF)
        return false;
      break;
    case '"':
      if (fputs("\\\"", output) == EOF)
        return false;
      break;
    case '\n':
      if (fputs("\\n", output) == EOF)
        return false;
      break;
    case '\r':
      if (fputs("\\r", output) == EOF)
        return false;
      break;
    case '\t':
      if (fputs("\\t", output) == EOF)
        return false;
      break;
    default:
      if (fputc(*cursor, output) == EOF)
        return false;
      break;
    }
  }
  return fputs("\"\n", output) != EOF;
}

static bool flags_config_apply_datum(flags_context_t *context, const flags_option_spec_t *option, toml_datum_t datum) {
  if (option->type == FLAGS_TYPE_CUSTOM) {
    char text[128];
    switch (datum.type) {
    case TOML_STRING:
      return flags_was_explicitly_set(context, option->name) || flags_set_text(context, option->name, datum.u.s, FLAGS_SOURCE_CONFIG);
    case TOML_INT64:
      snprintf(text, sizeof(text), "%lld", (long long)datum.u.int64);
      break;
    case TOML_FP64:
      snprintf(text, sizeof(text), "%.17g", datum.u.fp64);
      break;
    case TOML_BOOLEAN:
      snprintf(text, sizeof(text), "%s", datum.u.boolean ? "true" : "false");
      break;
    default:
      return false;
    }
    return flags_was_explicitly_set(context, option->name) || flags_set_text(context, option->name, text, FLAGS_SOURCE_CONFIG);
  }
  flags_value_t value = {0};
  switch (datum.type) {
  case TOML_STRING:
    /* TOML users commonly quote values that are numeric or boolean in the
     * hand-written configuration.  Reuse the normal text parser so config,
     * environment, and command-line sources accept the same value syntax. */
    if (option->type != FLAGS_TYPE_STRING)
      return flags_was_explicitly_set(context, option->name) || flags_set_text(context, option->name, datum.u.s, FLAGS_SOURCE_CONFIG);
    value.string = datum.u.s;
    break;
  case TOML_INT64:
    if (option->type != FLAGS_TYPE_INT64 && option->type != FLAGS_TYPE_UINT64 && option->type != FLAGS_TYPE_PORT)
      return false;
    value.integer = datum.u.int64;
    break;
  case TOML_FP64:
    if (option->type != FLAGS_TYPE_DOUBLE && option->type != FLAGS_TYPE_PERCENT && option->type != FLAGS_TYPE_DECIBEL)
      return false;
    value.number = datum.u.fp64;
    break;
  case TOML_BOOLEAN:
    if (option->type != FLAGS_TYPE_BOOL)
      return false;
    value.boolean = datum.u.boolean;
    break;
  default:
    return false;
  }
  if (flags_was_explicitly_set(context, option->name))
    return true;
  return flags_set(context, option->name, &value, FLAGS_SOURCE_CONFIG);
}

bool flags_config_load_toml(flags_context_t *context, const char *path) {
  if (!context || !path)
    return false;
  toml_result_t result = toml_parse_file_ex(path);
  if (!result.ok)
    return false;
  bool success = true;
  const flags_schema_t *schema = flags_context_schema(context);
  for (size_t i = 0; i < schema->option_count; ++i) {
    const flags_option_spec_t *option = &schema->options[i];
    toml_datum_t datum = toml_seek(result.toptab, option->config_key ? option->config_key : option->name);
    if (datum.type != TOML_UNKNOWN && !flags_config_apply_datum(context, option, datum)) {
      success = false;
      break;
    }
  }
  toml_free(result);
  return success;
}

bool flags_config_write_toml(const flags_context_t *context, const char *path) {
  if (!context || !path)
    return false;
  FILE *output = strcmp(path, "-") == 0 ? stdout : fopen(path, "wb");
  if (!output)
    return false;
  bool success = true;
  const flags_schema_t *schema = flags_context_schema(context);
  for (size_t i = 0; i < schema->option_count; ++i) {
    const flags_option_spec_t *option = &schema->options[i];
    flags_value_t value = {0};
    if (!flags_get(context, option->name, &value)) {
      success = false;
      break;
    }
    const char *key = option->config_key ? option->config_key : option->name;
    switch (option->type) {
    case FLAGS_TYPE_BOOL:
      fprintf(output, "%s = %s\n", key, value.boolean ? "true" : "false");
      break;
    case FLAGS_TYPE_INT64:
    case FLAGS_TYPE_PORT:
      fprintf(output, "%s = %lld\n", key, (long long)value.integer);
      break;
    case FLAGS_TYPE_UINT64:
    case FLAGS_TYPE_DURATION:
    case FLAGS_TYPE_SIZE:
    case FLAGS_TYPE_RATE:
      fprintf(output, "%s = %llu\n", key, (unsigned long long)value.unsigned_integer);
      break;
    case FLAGS_TYPE_DOUBLE:
    case FLAGS_TYPE_PERCENT:
    case FLAGS_TYPE_DECIBEL:
      fprintf(output, "%s = %.17g\n", key, value.number);
      break;
    default:
      if (!flags_config_write_string(output, key, value.string))
        success = false;
      break;
    }
  }
  if (output != stdout)
    success = fclose(output) == 0 && success;
  return success;
}
