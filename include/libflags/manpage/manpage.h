#pragma once
#include <stdio.h>
#include <libflags/flags.h>

typedef struct flags_manpage_metadata {
  /** Name shown in the generated title. */
  const char *program_name;
  /** Man-section identifier, such as 1. */
  const char *section;
  /** Short program description. */
  const char *description;
} flags_manpage_metadata_t;
/**
 * Writes a generic option section; application prose remains caller-owned.
 *
 * Options, aliases, argument requirements, metavariables, defaults, and
 * enum values are emitted from @p schema. Text is escaped for groff before it
 * is written to @p output.
 */
bool flags_manpage_write(const flags_schema_t *schema, const flags_manpage_metadata_t *metadata, FILE *output);
