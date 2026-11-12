/** @file
 * @brief Generic shell completion generation.
 */
#pragma once
#include <stdio.h>
#include <libflags/flags.h>

typedef enum flags_completion_shell { FLAGS_COMPLETION_BASH, FLAGS_COMPLETION_ZSH, FLAGS_COMPLETION_FISH } flags_completion_shell_t;
/** Writes a completion script using only schema metadata. */
bool flags_completion_write(const flags_schema_t *, const char *, flags_completion_shell_t, FILE *);
