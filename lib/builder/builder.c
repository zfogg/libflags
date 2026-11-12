#include <libflags/builder.h>

#include <stdlib.h>
#include <string.h>

struct flags_schema_builder {
  flags_option_spec_t *options;
  size_t option_count;
  size_t option_capacity;
  flags_positional_spec_t *positionals;
  size_t positional_count;
  size_t positional_capacity;
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
  free(builder->positionals);
  free(builder);
}

bool flags_schema_builder_add_option(flags_schema_builder_t *builder, const flags_option_spec_t *option) {
  if (!builder || !option || !option->name || !flags_builder_grow((void **)&builder->options, &builder->option_capacity, builder->option_count, sizeof(*builder->options)))
    return false;
  builder->options[builder->option_count++] = *option;
  return true;
}

bool flags_schema_builder_add_positional(flags_schema_builder_t *builder, const flags_positional_spec_t *positional) {
  if (!builder || !positional || !flags_builder_grow((void **)&builder->positionals, &builder->positional_capacity, builder->positional_count, sizeof(*builder->positionals)))
    return false;
  builder->positionals[builder->positional_count++] = *positional;
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
                                             .argument_order = builder->argument_order};
  return &mutable_builder->schema;
}
