#include <assert.h>
#include <libflags/flags.h>
#include <libflags/commands.h>
#include <libflags/values.h>
#include <string.h>
typedef struct {
  bool verbose;
  const char *file;
} values_t;
int main(void) {
  values_t v = {0};
  static const flags_option_spec_t o[] = {{.name = "verbose",
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
                                           .storage_size = sizeof(char *)}};
  flags_schema_t s = {o, 2, 0, 0};
  flags_context_t *c = flags_create(&s, &v, sizeof v);
  char *a[] = {"prog", "--file", "-", "--", "-literal", "--bad"};
  flags_parse_result_t r;
  assert(flags_parse(c, 6, a, &r));
  assert(r.options_ended);
  assert(r.positional_argc == 2);
  assert(!strcmp(r.positional_argv[0], "-literal"));
  flags_stream_value_t sv;
  assert(flags_classify_stream(&o[1], "-", &sv) && sv.kind == FLAGS_STREAM_STDIN);
  flags_environment_config_t environment = {.prefix = "TEST_", .derive_names = true};
  assert(flags_set_environment_config(c, &environment));
  char environment_name[64];
  assert(flags_environment_name(c, "file", environment_name, sizeof(environment_name)));
  assert(!strcmp(environment_name, "TEST_FILE"));
  flags_command_t *root = flags_command_create(NULL, "ascii-chat", "root");
  flags_command_t *mirror = flags_command_create(root, "mirror", "mirror mode");
  assert(root && mirror && flags_command_set_default(root, mirror));
  char *command_argv[] = {"prog", "--", "mirror"};
  int command_index = -1;
  bool used_default = false;
  assert(flags_resolve_command(root, 3, command_argv, &command_index, &used_default) == mirror);
  assert(used_default && command_index == -1);
  flags_command_destroy(root);
  bool enabled = false;
  uint64_t duration = 0;
  uint64_t size = 0;
  flags_decibel_t decibel = {0};
  assert(flags_parse_bool_relaxed("ON", &enabled) && enabled);
  assert(flags_parse_duration("500ms", &duration) && duration == 500000000ULL);
  assert(flags_parse_size("4KiB", &size) && size == 4096ULL);
  assert(flags_parse_decibel_or_auto("auto", &decibel) && decibel.automatic);
  flags_destroy(c);
  return 0;
}
