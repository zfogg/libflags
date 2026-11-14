#include <libflags/manpage/manpage.h>

#include <stdio.h>
#include <string.h>

bool flags_manpage_write(const flags_schema_t *schema, const flags_manpage_metadata_t *metadata, FILE *output) {
  if (!schema || !metadata || !metadata->program_name || !output)
    return false;
  fprintf(output, ".TH %s %s\n.SH NAME\n%s \\- generic command-line options\n", metadata->program_name, metadata->section ? metadata->section : "1", metadata->program_name);
  if (metadata->description)
    fprintf(output, ".SH DESCRIPTION\n%s\n", metadata->description);
  fprintf(output, ".SH OPTIONS\n");
  for (size_t i = 0; i < schema->option_count; ++i) {
    const flags_option_spec_t *option = &schema->options[i];
    fprintf(output, ".TP\n.B \\-\\-%s\n%s\n", option->name, option->secret ? "Sensitive value." : (option->description ? option->description : "Option value."));
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
    fprintf(output, ".TP\n.B %s\n%s\n", name, option->description ? option->description : "Option value.");
  }
  return !ferror(output);
}
