#include <libflags/flags.h>

#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/** Per-option mutable metadata owned by one context. */
typedef struct flags_stat {
  bool explicitly_set;
  bool changed_from_default;
  uint64_t change_count;
  uint64_t read_count;
  uint64_t write_count;
  struct timespec last_changed;
} flags_stat_t;

/** Private state for one independent parser instance. */
struct flags_context {
  const flags_schema_t *schema;
  void *storage;
  size_t storage_size;
  flags_stat_t *stats;
  flags_logging_config_t logging;
  flags_environment_config_t environment;
  uint64_t revision;
};

/** Finds an option by its canonical long name. */
static size_t flags_find(const flags_schema_t *schema, const char *name) {
  for (size_t i = 0; i < schema->option_count; ++i) {
    if (strcmp(schema->options[i].name, name) == 0)
      return i;
  }
  return SIZE_MAX;
}

/** Duplicates a string without requiring an application allocator. */
static char *flags_strdup(const char *text) {
  if (!text)
    return NULL;
  size_t length = strlen(text) + 1;
  char *copy = malloc(length);
  if (copy)
    memcpy(copy, text, length);
  return copy;
}

/** Records a monotonic-ish wall-clock change point for portable C11 builds. */
static void flags_now(struct timespec *out) {
  out->tv_sec = time(NULL);
  out->tv_nsec = 0;
}

/** Returns the byte size required by a schema type. */
static size_t flags_type_size(const flags_option_spec_t *option) {
  if (option->type == FLAGS_TYPE_BOOL)
    return sizeof(bool);
  if (option->type == FLAGS_TYPE_INT64 || option->type == FLAGS_TYPE_DURATION || option->type == FLAGS_TYPE_PORT)
    return sizeof(int64_t);
  if (option->type == FLAGS_TYPE_UINT64 || option->type == FLAGS_TYPE_SIZE || option->type == FLAGS_TYPE_RATE)
    return sizeof(uint64_t);
  if (option->type == FLAGS_TYPE_DOUBLE || option->type == FLAGS_TYPE_PERCENT || option->type == FLAGS_TYPE_DECIBEL)
    return sizeof(double);
  return sizeof(char *);
}

flags_context_t *flags_create(const flags_schema_t *schema, void *storage, size_t storage_size) {
  if (!schema || (!storage && storage_size != 0))
    return NULL;
  flags_context_t *context = calloc(1, sizeof(*context));
  if (!context)
    return NULL;
  context->schema = schema;
  context->storage = storage;
  context->storage_size = storage_size;
  context->stats = calloc(schema->option_count, sizeof(*context->stats));
  if (!context->stats) {
    free(context);
    return NULL;
  }
  return context;
}

void flags_destroy(flags_context_t *context) {
  if (!context)
    return;
  for (size_t i = 0; i < context->schema->option_count; ++i) {
    const flags_option_spec_t *option = &context->schema->options[i];
    if (option->dynamic_string && option->storage_size == sizeof(char *)) {
      char **value = (char **)((char *)context->storage + option->storage_offset);
      free(*value);
      *value = NULL;
    }
  }
  free(context->stats);
  free(context);
}

bool flags_validate(const flags_context_t *context) {
  if (!context || !context->schema)
    return false;
  for (size_t i = 0; i < context->schema->option_count; ++i) {
    const flags_option_spec_t *option = &context->schema->options[i];
    if (option->storage_offset > context->storage_size || option->storage_size > context->storage_size - option->storage_offset)
      return false;
    if (option->storage_size < flags_type_size(option))
      return false;
  }
  return true;
}

bool flags_set_defaults(flags_context_t *context) {
  if (!flags_validate(context))
    return false;
  for (size_t i = 0; i < context->schema->option_count; ++i) {
    const flags_option_spec_t *option = &context->schema->options[i];
    if (!option->default_value)
      continue;
    flags_value_t value = {0};
    memcpy(&value, option->default_value, option->storage_size < sizeof(value) ? option->storage_size : sizeof(value));
    if (!flags_set(context, option->name, &value, FLAGS_SOURCE_DEFAULT))
      return false;
    context->stats[i].explicitly_set = false;
    context->stats[i].change_count = 0;
    context->stats[i].write_count = 0;
  }
  return true;
}

bool flags_classify_stream(const flags_option_spec_t *option, const char *text, flags_stream_value_t *out) {
  if (!option || !text || !out)
    return false;
  out->text = text;
  out->kind = FLAGS_STREAM_FILE;
  if (strcmp(text, "-") != 0)
    return true;
  switch (option->dash_behavior) {
  case FLAGS_DASH_NOT_ALLOWED:
    return false;
  case FLAGS_DASH_STDIN:
    out->kind = FLAGS_STREAM_STDIN;
    break;
  case FLAGS_DASH_STDOUT:
    out->kind = FLAGS_STREAM_STDOUT;
    break;
  default:
    break;
  }
  return true;
}

bool flags_set(flags_context_t *context, const char *name, const flags_value_t *value, flags_source_t source) {
  if (!context || !name || !value || !flags_validate(context))
    return false;
  size_t index = flags_find(context->schema, name);
  if (index == SIZE_MAX)
    return false;
  const flags_option_spec_t *option = &context->schema->options[index];
  ++context->stats[index].write_count;
  if (option->validate && !option->validate(context, name, value, NULL, 0, option->user_data))
    return false;
  void *destination = (char *)context->storage + option->storage_offset;
  flags_value_t old = {0};
  memcpy(&old, destination, option->storage_size < sizeof(old) ? option->storage_size : sizeof(old));
  if (option->dynamic_string && option->storage_size == sizeof(char *)) {
    char **target = destination;
    char *copy = flags_strdup(value->string);
    if (value->string && !copy)
      return false;
    free(*target);
    *target = copy;
  } else {
    memcpy(destination, value, option->storage_size);
  }
  context->stats[index].explicitly_set = source != FLAGS_SOURCE_DEFAULT;
  context->stats[index].changed_from_default = source != FLAGS_SOURCE_DEFAULT;
  ++context->stats[index].change_count;
  ++context->revision;
  flags_now(&context->stats[index].last_changed);
  if (option->on_change)
    option->on_change(context, name, &old, value, option->user_data);
  return true;
}

bool flags_get(const flags_context_t *context, const char *name, flags_value_t *out) {
  if (!context || !name || !out)
    return false;
  size_t index = flags_find(context->schema, name);
  if (index == SIZE_MAX)
    return false;
  const flags_option_spec_t *option = &context->schema->options[index];
  memcpy(out, (const char *)context->storage + option->storage_offset, option->storage_size < sizeof(*out) ? option->storage_size : sizeof(*out));
  ++((flags_context_t *)context)->stats[index].read_count;
  return true;
}

bool flags_get_string(const flags_context_t *context, const char *name, char **out) {
  if (!out)
    return false;
  flags_value_t value = {0};
  if (!flags_get(context, name, &value))
    return false;
  *out = flags_strdup(value.string);
  return value.string == NULL || *out != NULL;
}

void flags_free_string(char *value) {
  free(value);
}

static const flags_option_spec_t *flags_lookup(const flags_schema_t *schema, const char *name) {
  for (size_t i = 0; i < schema->option_count; ++i) {
    const flags_option_spec_t *option = &schema->options[i];
    if (strcmp(option->name, name) == 0 || (option->short_name && name[0] == option->short_name && name[1] == '\0'))
      return option;
  }
  return NULL;
}

static bool flags_parse_value(flags_context_t *context, const flags_option_spec_t *option, const char *text, flags_value_t *out) {
  if (option->parse)
    return option->parse(context, text, out, option->user_data);
  char *end = NULL;
  switch (option->type) {
  case FLAGS_TYPE_BOOL:
    out->boolean = strcmp(text, "true") == 0 || strcmp(text, "1") == 0 || strcmp(text, "on") == 0;
    return out->boolean || strcmp(text, "false") == 0 || strcmp(text, "0") == 0 || strcmp(text, "off") == 0;
  case FLAGS_TYPE_INT64:
  case FLAGS_TYPE_DURATION:
  case FLAGS_TYPE_PORT:
    out->integer = strtoll(text, &end, 10);
    return end != text && *end == '\0';
  case FLAGS_TYPE_UINT64:
  case FLAGS_TYPE_SIZE:
  case FLAGS_TYPE_RATE:
    out->unsigned_integer = strtoull(text, &end, 10);
    return end != text && *end == '\0';
  case FLAGS_TYPE_DOUBLE:
  case FLAGS_TYPE_PERCENT:
  case FLAGS_TYPE_DECIBEL:
    out->number = strtod(text, &end);
    return end != text && (*end == '\0' || (option->type == FLAGS_TYPE_DECIBEL && strcmp(end, "dB") == 0));
  default:
    out->string = text;
    return true;
  }
}

bool flags_parse(flags_context_t *context, int argc, char **argv, flags_parse_result_t *result) {
  if (!context || !result)
    return false;
  memset(result, 0, sizeof(*result));
  bool ended = false;
  int first = argc;
  for (int i = 0; i < argc; ++i) {
    char *argument = argv[i];
    if (!ended && strcmp(argument, "--") == 0) {
      ended = true;
      if (first == argc)
        first = i + 1;
      continue;
    }
    if (!ended && argument[0] == '-' && argument[1] != '\0') {
      const char *name = argument + 1;
      if (*name == '-')
        ++name;
      const flags_option_spec_t *option = flags_lookup(context->schema, name);
      if (!option)
        return false;
      const char *text = NULL;
      if (option->argument_requirement != FLAGS_ARGUMENT_NONE) {
        if (argument[1] == '-' && argument[2] == '=')
          text = argument + 3;
        else if (i + 1 < argc)
          text = argv[++i];
        else if (option->argument_requirement == FLAGS_ARGUMENT_REQUIRED)
          return false;
      }
      flags_value_t value = {.boolean = true};
      if (text && !flags_parse_value(context, option, text, &value))
        return false;
      if (!flags_set(context, option->name, &value, FLAGS_SOURCE_COMMAND_LINE))
        return false;
      continue;
    }
    if (first == argc)
      first = i;
  }
  result->options_ended = ended;
  result->first_positional_index = first;
  result->positional_argc = first == argc ? 0 : argc - first;
  result->positional_argv = first == argc ? NULL : &argv[first];
  return true;
}

bool flags_was_explicitly_set(const flags_context_t *context, const char *name) {
  size_t i = context ? flags_find(context->schema, name) : SIZE_MAX;
  return i != SIZE_MAX && context->stats[i].explicitly_set;
}

uint64_t flags_change_count(const flags_context_t *context, const char *name) {
  size_t i = context ? flags_find(context->schema, name) : SIZE_MAX;
  return i == SIZE_MAX ? 0 : context->stats[i].change_count;
}

bool flags_changed_from_default(const flags_context_t *context, const char *name) {
  size_t i = context ? flags_find(context->schema, name) : SIZE_MAX;
  return i != SIZE_MAX && context->stats[i].changed_from_default;
}

uint64_t flags_read_count(const flags_context_t *context, const char *name) {
  size_t i = context ? flags_find(context->schema, name) : SIZE_MAX;
  return i == SIZE_MAX ? 0 : context->stats[i].read_count;
}

uint64_t flags_write_count(const flags_context_t *context, const char *name) {
  size_t i = context ? flags_find(context->schema, name) : SIZE_MAX;
  return i == SIZE_MAX ? 0 : context->stats[i].write_count;
}

bool flags_last_changed_at(const flags_context_t *context, const char *name, struct timespec *out) {
  size_t i = context ? flags_find(context->schema, name) : SIZE_MAX;
  if (i == SIZE_MAX || !out || context->stats[i].change_count == 0)
    return false;
  *out = context->stats[i].last_changed;
  return true;
}

bool flags_time_since_change(const flags_context_t *context, const char *name, uint64_t *out) {
  struct timespec then;
  struct timespec now;
  if (!out || !flags_last_changed_at(context, name, &then))
    return false;
  flags_now(&now);
  if (now.tv_nsec < then.tv_nsec) {
    now.tv_nsec += 1000000000L;
    --now.tv_sec;
  }
  *out = (uint64_t)(now.tv_sec - then.tv_sec) * 1000000000ULL + (uint64_t)(now.tv_nsec - then.tv_nsec);
  return true;
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

bool flags_set_environment_config(flags_context_t *context, const flags_environment_config_t *config) {
  if (!context)
    return false;
  context->environment = config ? *config : (flags_environment_config_t){0};
  return true;
}

bool flags_environment_name(const flags_context_t *context, const char *name, char *buffer, size_t buffer_size) {
  if (!context || !name || !buffer || buffer_size == 0)
    return false;
  size_t index = flags_find(context->schema, name);
  if (index == SIZE_MAX)
    return false;
  const flags_option_spec_t *option = &context->schema->options[index];
  const char *source = option->environment_name;
  char derived[256];
  if (!source && context->environment.derive_names) {
    const char *prefix = context->environment.prefix ? context->environment.prefix : "";
    size_t position = 0;
    while (prefix[position] && position + 1 < sizeof(derived))
      derived[position] = prefix[position], ++position;
    for (size_t i = 0; name[i] && position + 1 < sizeof(derived); ++i) {
      unsigned char character = (unsigned char)name[i];
      derived[position++] = (char)(character == '-' ? '_' : toupper(character));
    }
    derived[position] = '\0';
    source = derived;
  }
  if (!source || strlen(source) + 1 > buffer_size)
    return false;
  strcpy(buffer, source);
  return true;
}

bool flags_apply_environment(flags_context_t *context) {
  if (!context)
    return false;
  for (size_t i = 0; i < context->schema->option_count; ++i) {
    char name[256];
    if (!flags_environment_name(context, context->schema->options[i].name, name, sizeof(name)))
      continue;
    const char *text = getenv(name);
    if (!text)
      continue;
    flags_value_t value = {0};
    if (!flags_parse_value(context, &context->schema->options[i], text, &value) || !flags_set(context, context->schema->options[i].name, &value, FLAGS_SOURCE_ENVIRONMENT))
      return false;
  }
  return true;
}
