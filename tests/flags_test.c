#include <assert.h>
#include <libflags/flags.h>
#include <libflags/commands.h>
#include <libflags/values.h>
#include <string.h>
typedef struct {
  bool verbose;
  const char *file;
  int positional_count;
} values_t;

static bool count_positional(flags_context_t *context, const char *text, void *user_data) {
  (void)context;
  (void)text;
  int *count = user_data;
  ++*count;
  return true;
}

int main(void) {
  values_t v = {0};
  static const char *const verbose_aliases[] = {"debug"};
  static const flags_option_spec_t o[] = {{.name = "verbose",
                                           .aliases = verbose_aliases,
                                           .alias_count = 1,
                                           .short_name = 'v',
                                           .type = FLAGS_TYPE_BOOL,
                                           .argument_requirement = FLAGS_ARGUMENT_NONE,
                                           .dash_behavior = FLAGS_DASH_LITERAL,
                                           .storage_offset = offsetof(values_t, verbose),
                                           .storage_size = sizeof(bool)},
                                          {.name = "file",
                                           .short_name = 'f',
                                           .type = FLAGS_TYPE_STRING,
                                           .argument_requirement = FLAGS_ARGUMENT_REQUIRED,
                                           .dash_behavior = FLAGS_DASH_STDIN,
                                           .storage_offset = offsetof(values_t, file),
                                           .storage_size = sizeof(char *),
                                           .dynamic_string = true}};
  const flags_positional_spec_t positionals[] = {{.name = "files", .metavar = "FILE...", .arity = FLAGS_POSITIONAL_ZERO_OR_MORE, .parse = count_positional, .user_data = &v.positional_count}};
  flags_schema_t s = {o, 2, positionals, 1, FLAGS_POSITIONAL_STOP};
  flags_context_t *c = flags_create(&s, &v, sizeof v);
  char *a[] = {"prog", "--file", "-", "--", "-literal", "--bad"};
  flags_parse_result_t r;
  assert(flags_parse(c, 6, a, &r));
  assert(r.options_ended);
  assert(r.positional_argc == 2 && v.positional_count == 2);
  assert(!strcmp(r.positional_argv[0], "-literal"));
  assert(!strcmp(v.file, "-"));
  assert(flags_parse_positionals(c, 0, NULL));
  struct timespec last_read;
  flags_value_t read_value;
  assert(flags_get(c, "verbose", &read_value) && flags_last_read_at(c, "verbose", &last_read));
  int direct_positional_count = 0;
  const flags_positional_spec_t direct_positionals[] = {
      {.name = "items", .metavar = "ITEM...", .arity = FLAGS_POSITIONAL_ONE_OR_MORE, .parse = count_positional, .user_data = &direct_positional_count}};
  flags_schema_t direct_schema = {NULL, 0, direct_positionals, 1, FLAGS_POSITIONAL_STOP};
  flags_context_t *direct_context = flags_create(&direct_schema, NULL, 0);
  char *direct_argv[] = {"first", "second"};
  assert(direct_context && flags_parse_positionals(direct_context, 2, direct_argv));
  assert(direct_positional_count == 2);
  flags_destroy(direct_context);
  values_t optional_values = {0};
  static const flags_option_spec_t optional_option[] = {
      {.name = "label", .type = FLAGS_TYPE_BOOL, .argument_requirement = FLAGS_ARGUMENT_OPTIONAL, .storage_offset = offsetof(values_t, verbose), .storage_size = sizeof(bool)}};
  static const flags_positional_spec_t optional_positionals[] = {{.name = "rest", .arity = FLAGS_POSITIONAL_ZERO_OR_MORE}};
  flags_schema_t optional_schema = {optional_option, 1, optional_positionals, 1, FLAGS_POSITIONAL_PERMUTE};
  flags_context_t *optional_context = flags_create(&optional_schema, &optional_values, sizeof optional_values);
  char *optional_argv[] = {"prog", "--label", "--", "-literal"};
  assert(optional_context && flags_parse(optional_context, 4, optional_argv, &r));
  assert(r.options_ended && r.positional_argc == 1 && !strcmp(r.positional_argv[0], "-literal"));
  flags_destroy(optional_context);
  flags_stream_value_t sv;
  assert(flags_classify_stream(&o[1], "-", &sv) && sv.kind == FLAGS_STREAM_STDIN);
  flags_environment_config_t environment = {.prefix = "TEST_", .derive_names = true};
  assert(flags_set_environment_config(c, &environment));
  environment.prefix = "MUTATED_";
  char environment_name[64];
  assert(flags_environment_name(c, "file", environment_name, sizeof(environment_name)));
  assert(!strcmp(environment_name, "TEST_FILE"));
  assert(flags_derive_environment_name("ASCII_CHAT_", "render-file", environment_name, sizeof(environment_name)));
  assert(!strcmp(environment_name, "ASCII_CHAT_RENDER_FILE"));
  flags_value_t precedence = {.boolean = true};
  assert(flags_set(c, "verbose", &precedence, FLAGS_SOURCE_ENVIRONMENT));
  precedence.boolean = false;
  assert(flags_set(c, "verbose", &precedence, FLAGS_SOURCE_CONFIG));
  assert(flags_set(c, "verbose", &precedence, FLAGS_SOURCE_COMMAND_LINE));
  precedence.boolean = true;
  assert(!flags_set(c, "verbose", &precedence, FLAGS_SOURCE_ENVIRONMENT));
  assert(flags_parse(c, 2, (char *[]){"prog", "--debug"}, &r));
  assert(flags_was_explicitly_set(c, "verbose"));
  flags_command_t *root = flags_command_create(NULL, "ascii-chat", "root");
  flags_command_t *mirror = flags_command_create(root, "mirror", "mirror mode");
  assert(root && mirror && flags_command_set_default(root, mirror));
  assert(flags_command_set_implicit_for_option(root, mirror, "file"));
  static const flags_option_spec_t port_option = {.name = "port", .argument_requirement = FLAGS_ARGUMENT_REQUIRED};
  assert(flags_command_add_option(root, &port_option));
  char *command_argv[] = {"prog", "--", "mirror"};
  int command_index = -1;
  bool used_default = false;
  assert(flags_resolve_command(root, 3, command_argv, &command_index, &used_default) == mirror);
  assert(used_default && command_index == -1);
  char *implicit_argv[] = {"prog", "--file", "movie.mp4"};
  flags_command_resolution_t resolution;
  assert(flags_resolve_command_ex(root, 3, implicit_argv, &resolution));
  assert(resolution.command == mirror && resolution.used_implicit_command && !resolution.options_ended);
  flags_command_t *client = flags_command_create(root, "client", "client mode");
  char *explicit_argv[] = {"prog", "--port", "8080", "client"};
  assert(client && flags_resolve_command_ex(root, 4, explicit_argv, &resolution));
  assert(resolution.command == client && resolution.command_index == 3);
  assert(resolution.first_positional_index == 3);
  flags_command_destroy(root);
  bool enabled = false;
  uint64_t duration = 0;
  uint64_t size = 0;
  flags_decibel_t decibel = {0};
  assert(flags_parse_bool_relaxed("ON", &enabled) && enabled);
  assert(flags_parse_duration("500ms", &duration) && duration == 500000000ULL);
  assert(flags_parse_size("4KiB", &size) && size == 4096ULL);
  assert(flags_parse_decibel_or_auto("auto", &decibel) && decibel.automatic);
  flags_string_list_t strings = {0};
  assert(flags_parse_string_list("one, two ,three", &strings) && strings.count == 3);
  assert(!strcmp(strings.items[1], "two"));
  flags_free_string_list(&strings);
  flags_int_list_t integers = {0};
  assert(flags_parse_int_list("1, -2, 3", &integers) && integers.count == 3 && integers.items[1] == -2);
  flags_free_int_list(&integers);
  flags_value_t update_value = {.boolean = true};
  flags_update_t update = {.name = "verbose", .value = update_value};
  uint64_t revision = 0;
  assert(flags_runtime_apply_batch(c, &update, 1, flags_runtime_revision(c), FLAGS_SOURCE_RUNTIME, &revision));
  assert(v.verbose && revision == flags_runtime_revision(c));
  flags_update_t failed_updates[] = {{.name = "verbose", .value = {.boolean = false}}, {.name = "missing", .value = {.boolean = false}}};
  assert(!flags_runtime_apply_batch(c, failed_updates, 2, flags_runtime_revision(c), FLAGS_SOURCE_RUNTIME, NULL));
  assert(v.verbose);
  flags_destroy(c);
  return 0;
}
