#pragma once
#include <stdio.h>
#include <libflags/flags.h>

/**
 * @brief Metadata used by flags_manpage_write().
 */
typedef struct flags_manpage_metadata {
  /** Name shown in the generated title. */
  const char *program_name;
  /** Man-section identifier, such as 1. */
  const char *section;
  /** Short program description. */
  const char *description;
} flags_manpage_metadata_t;
/**
 * @brief Writes a generic option section; application prose remains caller-owned.
 *
 * Options, aliases, argument requirements, metavariables, defaults, and
 * enum values are emitted from @p schema. Text is escaped for groff before it
 * is written to @p output.
 * @param schema Option schema to document.
 * @param metadata Program name, section, and description.
 * @param output Caller-owned output stream.
 * @return `true` when generation succeeds.
 * @note The caller opens and closes @p output.
 */
bool flags_manpage_write(const flags_schema_t *schema, const flags_manpage_metadata_t *metadata, FILE *output);

/**
 * @brief Writes an environment-variable section from a schema.
 * @param schema Option schema whose environment metadata is documented.
 * @param prefix Prefix for derived names, or NULL to omit derived names.
 * @param output Destination stream.
 * @return `true` when generation succeeds.
 * @note Explicit environment names are always emitted; derived names are
 * emitted when @p prefix is non-NULL.
 */
bool flags_manpage_write_environment(const flags_schema_t *schema, const char *prefix, FILE *output);
