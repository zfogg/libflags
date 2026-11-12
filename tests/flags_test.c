#include <assert.h>
#include <libflags/flags.h>
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
  char *a[] = {"--file", "-", "--", "-literal", "--bad"};
  flags_parse_result_t r;
  assert(flags_parse(c, 5, a, &r));
  assert(r.options_ended);
  assert(r.positional_argc == 2);
  assert(!strcmp(r.positional_argv[0], "-literal"));
  flags_stream_value_t sv;
  assert(flags_classify_stream(&o[1], "-", &sv) && sv.kind == FLAGS_STREAM_STDIN);
  flags_destroy(c);
  return 0;
}
