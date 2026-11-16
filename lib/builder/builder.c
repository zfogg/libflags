#include <libflags/builder.h>
#include <libplatform/util/string.h>

#include <stdlib.h>
#include <string.h>

struct flags_schema_builder {
  flags_option_spec_t *options;
  char **owned_config_keys;
  size_t owned_config_key_count;
  size_t owned_config_key_capacity;
  size_t option_count;
  size_t option_capacity;
  flags_positional_spec_t *positionals;
  size_t positional_count;
  size_t positional_capacity;
  flags_dependency_t *dependencies;
  size_t dependency_count;
  size_t dependency_capacity;
  flags_argument_order_t argument_order;
  flags_schema_t schema;
};

static bool flags_builder_grow(void **array, size_t *capacity, size_t count, size_t element_size) {
  if (count < *capacity)
    return true;
  size_t next = *capacity ? *capacity * 2 : 8;
  if (next < count + 1 || next > SIZE_MAX / element_size)
    return false;
  void *grown = realloc(*array, next * element_size);
  if (!grown)
    return false;
  *array = grown;
  *capacity = next;
  return true;
}

flags_schema_builder_t *flags_schema_builder_create(void) {
  flags_schema_builder_t *builder = calloc(1, sizeof(*builder));
  if (builder)
    builder->argument_order = FLAGS_POSITIONAL_STOP;
  return builder;
}

void flags_schema_builder_destroy(flags_schema_builder_t *builder) {
  if (!builder)
    return;
  free(builder->options);
  for (size_t i = 0; i < builder->owned_config_key_count; ++i)
    free(builder->owned_config_keys[i]);
  free(builder->owned_config_keys);
  free(builder->positionals);
  free(builder->dependencies);
  free(builder);
}

bool flags_schema_builder_add_option(flags_schema_builder_t *builder, const flags_option_spec_t *option) {
  if (!builder || !option || !option->name || !flags_builder_grow((void **)&builder->options, &builder->option_capacity, builder->option_count, sizeof(*builder->options)))
    return false;
  flags_option_spec_t copy = *option;
  if (option->config_key) {
    char *config_key = platform_strdup(option->config_key);
    if (!config_key)
      return false;
    if (!flags_builder_grow((void **)&builder->owned_config_keys, &builder->owned_config_key_capacity, builder->owned_config_key_count, sizeof(*builder->owned_config_keys))) {
      free(config_key);
      return false;
    }
    builder->owned_config_keys[builder->owned_config_key_count++] = config_key;
    copy.config_key = config_key;
  }
  builder->options[builder->option_count++] = copy;
  return true;
}

bool flags_schema_builder_add_positional(flags_schema_builder_t *builder, const flags_positional_spec_t *positional) {
  if (!builder || !positional || !flags_builder_grow((void **)&builder->positionals, &builder->positional_capacity, builder->positional_count, sizeof(*builder->positionals)))
    return false;
  builder->positionals[builder->positional_count++] = *positional;
  return true;
}

bool flags_schema_builder_add_dependency(flags_schema_builder_t *builder, const flags_dependency_t *dependency) {
  if (!builder || !dependency || !dependency->option_name || !dependency->related_option || dependency->type < FLAGS_DEPENDENCY_REQUIRES || dependency->type > FLAGS_DEPENDENCY_IMPLIES ||
      !flags_builder_grow((void **)&builder->dependencies, &builder->dependency_capacity, builder->dependency_count, sizeof(*builder->dependencies)))
    return false;
  builder->dependencies[builder->dependency_count++] = *dependency;
  return true;
}

bool flags_schema_builder_set_argument_order(flags_schema_builder_t *builder, flags_argument_order_t order) {
  if (!builder || order < FLAGS_POSITIONAL_PERMUTE || order > FLAGS_POSITIONAL_ONLY_AFTER_TERMINATOR)
    return false;
  builder->argument_order = order;
  return true;
}

const flags_schema_t *flags_schema_builder_schema(const flags_schema_builder_t *builder) {
  if (!builder)
    return NULL;
  flags_schema_builder_t *mutable_builder = (flags_schema_builder_t *)builder;
  mutable_builder->schema = (flags_schema_t){.options = builder->options,
                                             .option_count = builder->option_count,
                                             .positionals = builder->positionals,
                                             .positional_count = builder->positional_count,
                                             .argument_order = builder->argument_order,
                                             .dependencies = builder->dependencies,
                                             .dependency_count = builder->dependency_count};
  return &mutable_builder->schema;
}
