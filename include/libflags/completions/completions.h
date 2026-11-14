/** @file
 * @brief Generic shell completion generation.
 */
#pragma once
#include <stdio.h>
#include <libflags/flags.h>

/** Shell dialect understood by flags_completion_write(). */
typedef enum flags_completion_shell {
  /** Bash completion function syntax. */
  FLAGS_COMPLETION_BASH,
  /** Zsh completion function syntax. */
  FLAGS_COMPLETION_ZSH,
  /** Fish completion command syntax. */
  FLAGS_COMPLETION_FISH
} flags_completion_shell_t;

/**
 * Writes a completion script using only schema metadata.
 *
 * The generated script includes option names, aliases, value placeholders,
 * enum choices, and positional metadata available in the schema. The caller
 * supplies the application command name and retains ownership of @p output.
 */
bool flags_completion_write(const flags_schema_t *schema, const char *command_name, flags_completion_shell_t shell, FILE *output);
