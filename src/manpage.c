#include <libflags/manpage/manpage.h>

#include <stdio.h>

bool flags_manpage_write(const flags_schema_t *schema, const flags_manpage_metadata_t *metadata, FILE *output) {
  if (!schema || !metadata || !metadata->program_name || !output)
    return false;
  fprintf(output, ".TH %s %s\n.SH NAME\n%s \\- generic command-line options\n", metadata->program_name, metadata->section ? metadata->section : "1", metadata->program_name);
  if (metadata->description)
    fprintf(output, ".SH DESCRIPTION\n%s\n", metadata->description);
  fprintf(output, ".SH OPTIONS\n");
  for (size_t i = 0; i < schema->option_count; ++i) {
    const flags_option_spec_t *option = &schema->options[i];
    fprintf(output, ".TP\n.B \\\-\\-%s\n%s\n", option->name, option->secret ? "Sensitive value." : "Option value.");
  }
  return !ferror(output);
}
