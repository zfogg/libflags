#include <libflags/flags.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
  bool explicit_set;
  uint64_t changes;
} stat_t;
struct flags_context {
  const flags_schema_t *schema;
  void *storage;
  size_t storage_size;
  stat_t *stats;
};
static size_t find(const flags_schema_t *s, const char *n) {
  for (size_t i = 0; i < s->option_count; i++)
    if (!strcmp(s->options[i].name, n))
      return i;
  return SIZE_MAX;
}
flags_context_t *flags_create(const flags_schema_t *s, void *storage, size_t size) {
  if (!s || !storage)
    return NULL;
  flags_context_t *c = calloc(1, sizeof *c);
  if (!c)
    return NULL;
  c->schema = s;
  c->storage = storage;
  c->storage_size = size;
  c->stats = calloc(s->option_count, sizeof *c->stats);
  if (!c->stats) {
    free(c);
    return NULL;
  }
  return c;
}
void flags_destroy(flags_context_t *c) {
  if (c) {
    free(c->stats);
    free(c);
  }
}
bool flags_set_defaults(flags_context_t *c) {
  if (!c)
    return false;
  for (size_t i = 0; i < c->schema->option_count; i++) {
    const flags_option_spec_t *o = &c->schema->options[i];
    if (o->default_value)
      memcpy((char *)c->storage + o->storage_offset, o->default_value, o->storage_size);
  }
  return true;
}
bool flags_classify_stream(const flags_option_spec_t *o, const char *t, flags_stream_value_t *out) {
  if (!o || !t || !out)
    return false;
  out->text = t;
  out->kind = FLAGS_STREAM_FILE;
  if (!strcmp(t, "-")) {
    if (o->dash_behavior == FLAGS_DASH_NOT_ALLOWED)
      return false;
    if (o->dash_behavior == FLAGS_DASH_STDIN)
      out->kind = FLAGS_STREAM_STDIN;
    else if (o->dash_behavior == FLAGS_DASH_STDOUT)
      out->kind = FLAGS_STREAM_STDOUT;
  }
  return true;
}
bool flags_set(flags_context_t *c, const char *n, const flags_value_t *v, flags_source_t source) {
  (void)source;
  if (!c || !n || !v)
    return false;
  size_t i = find(c->schema, n);
  if (i == SIZE_MAX)
    return false;
  const flags_option_spec_t *o = &c->schema->options[i];
  memcpy((char *)c->storage + o->storage_offset, v, o->storage_size);
  c->stats[i].explicit_set = true;
  c->stats[i].changes++;
  return true;
}
bool flags_get(const flags_context_t *c, const char *n, flags_value_t *out) {
  if (!c || !n || !out)
    return false;
  size_t i = find(c->schema, n);
  if (i == SIZE_MAX)
    return false;
  memcpy(out, (char *)c->storage + c->schema->options[i].storage_offset, c->schema->options[i].storage_size);
  return true;
}
static const flags_option_spec_t *lookup(const flags_schema_t *s, const char *n) {
  for (size_t i = 0; i < s->option_count; i++)
    if (!strcmp(s->options[i].name, n) || (s->options[i].short_name && n[0] == s->options[i].short_name && !n[1]))
      return &s->options[i];
  return NULL;
}
bool flags_parse(flags_context_t *c, int argc, char **argv, flags_parse_result_t *r) {
  if (!c || !r)
    return false;
  memset(r, 0, sizeof *r);
  bool ended = false;
  int first = argc;
  for (int i = 0; i < argc; i++) {
    char *a = argv[i];
    if (!ended && !strcmp(a, "--")) {
      ended = true;
      continue;
    }
    if (!ended && a[0] == '-' && a[1]) {
      const char *n = a + 1;
      if (*n == '-')
        n++;
      const flags_option_spec_t *o = lookup(c->schema, n);
      if (!o)
        return false;
      const char *v = NULL;
      if (o->argument_requirement != FLAGS_ARGUMENT_NONE) {
        if (a[1] == '-' && a[2] == '=')
          v = a + 3;
        else if (i + 1 < argc)
          v = argv[++i];
        else
          return false;
      }
      if (v) {
        flags_stream_value_t sv;
        if (!flags_classify_stream(o, v, &sv))
          return false;
        flags_value_t x = {.string = v};
        if (o->type == FLAGS_TYPE_BOOL)
          x.boolean = !strcmp(v, "true") || !strcmp(v, "1");
        else if (o->type == FLAGS_TYPE_INT64)
          x.integer = strtoll(v, NULL, 10);
        if (!flags_set(c, o->name, &x, FLAGS_SOURCE_COMMAND_LINE))
          return false;
      } else {
        flags_value_t x = {.boolean = true};
        if (!flags_set(c, o->name, &x, FLAGS_SOURCE_COMMAND_LINE))
          return false;
      }
      continue;
    }
    if (first == argc)
      first = i;
  }
  r->options_ended = ended;
  r->first_positional_index = first;
  r->positional_argc = first == argc ? 0 : argc - first;
  r->positional_argv = first == argc ? NULL : &argv[first];
  return true;
}
bool flags_validate(const flags_context_t *c) { return c && c->storage_size > 0; }
bool flags_was_explicitly_set(const flags_context_t *c, const char *n) {
  size_t i = c ? find(c->schema, n) : SIZE_MAX;
  return i != SIZE_MAX && c->stats[i].explicit_set;
}
uint64_t flags_change_count(const flags_context_t *c, const char *n) {
  size_t i = c ? find(c->schema, n) : SIZE_MAX;
  return i == SIZE_MAX ? 0 : c->stats[i].changes;
}
