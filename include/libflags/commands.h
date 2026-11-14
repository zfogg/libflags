/** @file
 * @brief Nested command schemas and command resolution.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <libflags/flags.h>

/** An opaque command node in a schema tree. */
typedef struct flags_command flags_command_t;

/** Result of resolving a root command and its command-line boundary. */
typedef struct flags_command_resolution {
  /** Selected command. */
  flags_command_t *command;
  /** argv index, or -1 when defaulted. */
  int command_index;
  /** Whether root default selection was used. */
  bool used_default_command;
  /** Whether an option selected the command. */
  bool used_implicit_command;
  /** Whether `--` occurred before resolution ended. */
  bool options_ended;
  /** First non-option candidate, or -1. */
  int first_positional_index;
} flags_command_resolution_t;

/** Creates a command below an optional parent command. */
flags_command_t *flags_command_create(flags_command_t *parent, const char *name, const char *description);
/** Destroys a command tree and its owned command metadata. */
void flags_command_destroy(flags_command_t *command);
/** Registers an option descriptor on a command. */
bool flags_command_add_option(flags_command_t *command, const flags_option_spec_t *spec);
/** Sets the command callback invoked by the application. */
bool flags_command_set_handler(flags_command_t *command, int (*handler)(flags_context_t *, void *), void *user_data);
/** Marks a descendant as the root's default command. */
bool flags_command_set_default(flags_command_t *root, flags_command_t *command);
/** Selects a command implicitly when the named root option is present. */
bool flags_command_set_implicit_for_option(flags_command_t *root, flags_command_t *command, const char *option_name);
/** Resolves a command name from argv without interpreting positional data after `--`. */
flags_command_t *flags_resolve_command(flags_command_t *root, int argc, char **argv, int *command_index, bool *used_default);
/** Resolves a command and returns all resolution metadata. */
bool flags_resolve_command_ex(flags_command_t *root, int argc, char **argv, flags_command_resolution_t *resolution);
/** Returns the command's canonical name. */
const char *flags_command_name(const flags_command_t *command);
/** Returns the command's parent, or NULL for a root command. */
flags_command_t *flags_command_parent(const flags_command_t *command);
