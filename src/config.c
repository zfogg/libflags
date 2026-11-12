#include <libflags/config.h>
#include <tomlc17.h>

#include <stdio.h>
#include <string.h>

static bool flags_config_apply_datum(flags_context_t *context, const flags_option_spec_t *option, toml_datum_t datum) {
  flags_value_t value = {0};
  switch (datum.type) {
  case TOML_STRING:
    value.string = datum.u.s;
    break;
  case TOML_INT64:
    value.integer = datum.u.int64;
    break;
  case TOML_FP64:
    value.number = datum.u.fp64;
    break;
  case TOML_BOOLEAN:
    value.boolean = datum.u.boolean;
    break;
  default:
    return false;
  }
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
    toml_datum_t datum = toml_seek(result.toptab, option->name);
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
    switch (option->type) {
    case FLAGS_TYPE_BOOL:
      fprintf(output, "%s = %s\n", option->name, value.boolean ? "true" : "false");
      break;
    case FLAGS_TYPE_INT64:
    case FLAGS_TYPE_PORT:
      fprintf(output, "%s = %lld\n", option->name, (long long)value.integer);
      break;
    case FLAGS_TYPE_UINT64:
    case FLAGS_TYPE_DURATION:
    case FLAGS_TYPE_SIZE:
    case FLAGS_TYPE_RATE:
      fprintf(output, "%s = %llu\n", option->name, (unsigned long long)value.unsigned_integer);
      break;
    case FLAGS_TYPE_DOUBLE:
    case FLAGS_TYPE_PERCENT:
    case FLAGS_TYPE_DECIBEL:
      fprintf(output, "%s = %.17g\n", option->name, value.number);
      break;
    default:
      if (value.string)
        fprintf(output, "%s = \"%s\"\n", option->name, value.string);
      break;
    }
  }
  if (output != stdout)
    success = fclose(output) == 0 && success;
  return success;
}
