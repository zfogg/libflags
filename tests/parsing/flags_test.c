#include <assert.h>
#include <libflags/flags.h>
#include <libflags/commands.h>
#include <libflags/values.h>
#include <libflags/config.h>
#include <libflags/completions/completions.h>
#include <libflags/manpage/manpage.h>
#include <libflags/builder.h>
#include <libflags/util/ip.h>
#include <libflags/util/url.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
  bool verbose;
  const char *file;
  int positional_count;
} values_t;

typedef struct {
  char name[32];
} fixed_values_t;

static bool count_positional(flags_context_t *context, const char *text, void *user_data) {
  (void)context;
  (void)text;
  int *count = user_data;
  ++*count;
  return true;
}

static void capture_log(const flags_log_event_t *event, void *user_data) {
  char *message = user_data;
  strcpy(message, event->message);
}

int main(void) {
  char address[64];
  uint16_t port = 0;
  assert(flags_is_valid_ipv4("192.0.2.1"));
  assert(flags_is_valid_ipv6("[2001:db8::1]"));
  assert(flags_parse_ip_with_port("[::1]:27224", address, sizeof address, &port) == 0);
  assert(!strcmp(address, "::1") && port == 27224);
  assert(flags_parse_address_with_optional_port("example.com", address, sizeof address, &port, 27224) == 0);
  assert(!strcmp(address, "example.com") && port == 27224);
  assert(flags_url_is_websocket_scheme("wss"));
  assert(flags_url_is_tcp_scheme("tcp"));

  fixed_values_t fixed_values = {0};
  static const flags_option_spec_t fixed_option = {.name = "name", .type = FLAGS_TYPE_STRING, .storage_offset = offsetof(fixed_values_t, name), .storage_size = sizeof fixed_values.name};
  const flags_schema_t fixed_schema = {.options = &fixed_option, .option_count = 1};
  flags_context_t *fixed_context = flags_create(&fixed_schema, &fixed_values, sizeof fixed_values);
  flags_value_t fixed_text = {.string = "fixed value"};
  assert(fixed_context && flags_set(fixed_context, "name", &fixed_text, FLAGS_SOURCE_RUNTIME));
  flags_value_t fixed_read = {0};
  assert(flags_get(fixed_context, "name", &fixed_read) && !strcmp(fixed_read.string, "fixed value"));
  flags_destroy(fixed_context);

  flags_schema_builder_t *builder = flags_schema_builder_create();
  flags_option_spec_t builder_option = {.name = "builder-option", .type = FLAGS_TYPE_BOOL};
  assert(builder && flags_schema_builder_add_option(builder, &builder_option));
  const flags_schema_t *built_schema = flags_schema_builder_schema(builder);
  assert(built_schema && built_schema->option_count == 1);
  flags_schema_builder_destroy(builder);
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
  static const flags_option_spec_t secret_option = {
      .name = "password", .type = FLAGS_TYPE_STRING, .secret = true, .storage_offset = offsetof(values_t, file), .storage_size = sizeof(char *), .dynamic_string = true};
  const flags_positional_spec_t positionals[] = {{.name = "files", .metavar = "FILE...", .arity = FLAGS_POSITIONAL_ZERO_OR_MORE, .parse = count_positional, .user_data = &v.positional_count}};
  flags_schema_t s = {o, 2, positionals, 1, FLAGS_POSITIONAL_STOP};
  flags_context_t *c = flags_create(&s, &v, sizeof v);
  values_t secret_values = {0};
  const flags_schema_t secret_schema = {.options = &secret_option, .option_count = 1, .argument_order = FLAGS_POSITIONAL_STOP};
  flags_context_t *secret_context = flags_create(&secret_schema, &secret_values, sizeof secret_values);
  char log_message[128] = {0};
  assert(secret_context && flags_set_logging(secret_context, &(flags_logging_config_t){.callback = capture_log, .user_data = log_message, .minimum_level = FLAGS_LOG_TRACE}));
  flags_log_value(secret_context, FLAGS_LOG_INFO, FLAGS_LOG_CONFIG, "password", "do-not-leak");
  assert(strstr(log_message, "<redacted>") && !strstr(log_message, "do-not-leak"));
  flags_destroy(secret_context);
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
  flags_option_spec_t stdout_option = o[1];
  stdout_option.dash_behavior = FLAGS_DASH_STDOUT;
  assert(flags_classify_stream(&stdout_option, "-", &sv) && sv.kind == FLAGS_STREAM_STDOUT);
  stdout_option.dash_behavior = FLAGS_DASH_NOT_ALLOWED;
  assert(!flags_classify_stream(&stdout_option, "-", &sv));
  flags_environment_config_t environment = {.prefix = "TEST_", .derive_names = true};
  assert(flags_set_environment_config(c, &environment));
  environment.prefix = "MUTATED_";
  char environment_name[64];
  assert(flags_environment_name(c, "file", environment_name, sizeof(environment_name)));
  assert(!strcmp(environment_name, "TEST_FILE"));
  assert(flags_derive_environment_name("ASCII_CHAT_", "render-file", environment_name, sizeof(environment_name)));
  assert(!strcmp(environment_name, "ASCII_CHAT_RENDER_FILE"));
  flags_value_t config_file = {.string = "quoted\\path\nnext\tline"};
  assert(flags_set(c, "file", &config_file, FLAGS_SOURCE_RUNTIME));
  assert(flags_config_write_toml(c, "libflags-test.toml"));
  values_t config_values = {0};
  flags_context_t *config_context = flags_create(&s, &config_values, sizeof config_values);
  assert(config_context && flags_config_load_toml(config_context, "libflags-test.toml"));
  assert(config_values.file && !strcmp(config_values.file, config_file.string));
  flags_destroy_preserving_storage(config_context);
  assert(config_values.file && !strcmp(config_values.file, config_file.string));
  free((void *)config_values.file);
  remove("libflags-test.toml");
  flags_value_t precedence = {.boolean = true};
  assert(flags_set(c, "verbose", &precedence, FLAGS_SOURCE_ENVIRONMENT));
  precedence.boolean = false;
  assert(flags_set(c, "verbose", &precedence, FLAGS_SOURCE_CONFIG));
  assert(flags_set(c, "verbose", &precedence, FLAGS_SOURCE_COMMAND_LINE));
  precedence.boolean = true;
  assert(!flags_set(c, "verbose", &precedence, FLAGS_SOURCE_ENVIRONMENT));
  assert(flags_parse(c, 2, (char *[]){"prog", "--debug"}, &r));
  assert(flags_was_explicitly_set(c, "verbose"));
  FILE *generated = tmpfile();
  assert(generated);
  assert(flags_completion_write(&s, "example", FLAGS_COMPLETION_BASH, generated));
  fflush(generated);
  rewind(generated);
  char completion_text[4096] = {0};
  assert(fread(completion_text, 1, sizeof(completion_text) - 1, generated) > 0);
  assert(strstr(completion_text, "-v") != NULL);
  assert(strstr(completion_text, "--debug") != NULL);
  assert(flags_manpage_write(&s, &(flags_manpage_metadata_t){.program_name = "example"}, generated));
  fclose(generated);
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
  uint64_t elapsed = 0;
  struct timespec changed_at;
  assert(flags_last_changed_at(c, "verbose", &changed_at));
  assert(flags_time_since_change(c, "verbose", &elapsed));
  flags_update_t failed_updates[] = {{.name = "verbose", .value = {.boolean = false}}, {.name = "missing", .value = {.boolean = false}}};
  assert(!flags_runtime_apply_batch(c, failed_updates, 2, flags_runtime_revision(c), FLAGS_SOURCE_RUNTIME, NULL));
  assert(v.verbose);
  flags_destroy(c);
  return 0;
}
