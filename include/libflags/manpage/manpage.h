/** @file
 * @brief Generic groff manpage generation.
 *
 * The generator owns the schema-derived option section. Applications retain
 * ownership of command-specific prose and may compose it with the generated
 * output using the formatter and merge facades.
 */
#pragma once
#include <stdio.h>
#include <libflags/flags.h>

typedef struct flags_manpage_metadata {
  const char *program_name; /**< Name shown in the generated title. */
  const char *section;      /**< Man-section identifier, such as 1. */
  const char *description;  /**< Short program description. */
} flags_manpage_metadata_t;
/**
 * Writes a generic option section; application prose remains caller-owned.
 *
 * Options, aliases, argument requirements, metavariables, defaults, and
 * enum values are emitted from @p schema. Text is escaped for groff before it
 * is written to @p output.
 */
bool flags_manpage_write(const flags_schema_t *, const flags_manpage_metadata_t *, FILE *);
