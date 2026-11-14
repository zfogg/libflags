#include <libflags/manpage/manpage.h>
#include <libflags/manpage/formatter.h>

#include <stdio.h>
#include <string.h>

static const char *flags_manpage_type_placeholder(const flags_option_spec_t *option) {
  if (option->metavar)
    return option->metavar;
  switch (option->type) {
  case FLAGS_TYPE_BOOL:
    return NULL;
  case FLAGS_TYPE_INT64:
  case FLAGS_TYPE_UINT64:
  case FLAGS_TYPE_PORT:
    return "INTEGER";
  case FLAGS_TYPE_DOUBLE:
    return "NUMBER";
  default:
    return "VALUE";
  }
}

static void flags_manpage_write_option(const flags_option_spec_t *option, FILE *output) {
  fprintf(output, ".TP\n.B \\-\\-%s", option->name);
  if (option->short_name)
    fprintf(output, " (\\-%c)", option->short_name);
  const char *placeholder = flags_manpage_type_placeholder(option);
  if (placeholder)
    fprintf(output, " \\fI%s\\fR", flags_manpage_escape_groff(placeholder));
  fputc('\n', output);
  fprintf(output, "%s", flags_manpage_escape_groff(option->description ? option->description : "Option value."));
  if (option->required)
    fputs(" [required]", output);
  fputc('\n', output);
}

bool flags_manpage_write(const flags_schema_t *schema, const flags_manpage_metadata_t *metadata, FILE *output) {
  if (!schema || !metadata || !metadata->program_name || !output)
    return false;
  fprintf(output, ".TH %s %s\n.SH NAME\n%s \\- generic command-line options\n", metadata->program_name, metadata->section ? metadata->section : "1", metadata->program_name);
  if (metadata->description)
    fprintf(output, ".SH DESCRIPTION\n%s\n", metadata->description);
  fprintf(output, ".SH OPTIONS\n");
  for (size_t i = 0; i < schema->option_count; ++i) {
    const flags_option_spec_t *option = &schema->options[i];
    flags_manpage_write_option(option, output);
  }
  return !ferror(output);
}

bool flags_manpage_write_environment(const flags_schema_t *schema, const char *prefix, FILE *output) {
  if (!schema || !output)
    return false;
  for (size_t i = 0; i < schema->option_count; ++i) {
    const flags_option_spec_t *option = &schema->options[i];
    char derived[256];
    const char *name = option->environment_name;
    if (!name && prefix && flags_derive_environment_name(prefix, option->name, derived, sizeof(derived)))
      name = derived;
    if (!name)
      continue;
    fprintf(output, ".TP\n.B %s\n%s\n", name, option->secret ? "Sensitive value." : (option->description ? option->description : "Option value."));
    for (size_t alias = 0; alias < option->environment_alias_count; ++alias)
      if (option->environment_aliases[alias])
        fprintf(output, ".br\n.B %s\n", option->environment_aliases[alias]);
  }
  return !ferror(output);
}
