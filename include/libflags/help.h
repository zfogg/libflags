/** @file
 * @brief Generic command-line help rendering.
 */
#pragma once

#include <stdio.h>
#include <libflags/flags.h>

/**
 * @brief Writes the generic option portion of a help document.
 * @param schema Option schema to render.
 * @param program_name Program name shown in the heading.
 * @param description Optional program description.
 * @param output Destination stream owned by the caller.
 * @return `true` when all output was written successfully.
 * @note Commands, modes, positional prose, and application-specific sections
 *       remain the caller's responsibility.
 */
bool flags_help_write_options(const flags_schema_t *schema, const char *program_name, const char *description, FILE *output);
