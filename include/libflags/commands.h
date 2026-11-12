/** @file
 * @brief Nested command schemas and command resolution.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <libflags/flags.h>

/** An opaque command node in a schema tree. */
typedef struct flags_command flags_command_t;

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
/** Resolves a command name from argv without interpreting positional data after `--`. */
flags_command_t *flags_resolve_command(flags_command_t *root, int argc, char **argv, int *command_index, bool *used_default);
/** Returns the command's canonical name. */
const char *flags_command_name(const flags_command_t *command);
/** Returns the command's parent, or NULL for a root command. */
flags_command_t *flags_command_parent(const flags_command_t *command);

