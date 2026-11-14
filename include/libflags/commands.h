/** @file
 * @brief Nested command schemas and command resolution.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <libflags/flags.h>

/**
 * @brief Opaque command node in a schema tree.
 */
typedef struct flags_command flags_command_t;

/**
 * @brief Result of resolving a root command and its command-line boundary.
 */
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

/**
 * @brief Creates a command below an optional parent command.
 * @param parent Optional parent command.
 * @param name Canonical command name.
 * @param description Help description.
 * @return New command, or NULL on failure.
 */
flags_command_t *flags_command_create(flags_command_t *parent, const char *name, const char *description);
/**
 * @brief Destroys a command tree and its owned command metadata.
 * @param command Root command to destroy.
 */
void flags_command_destroy(flags_command_t *command);
/**
 * @brief Registers an option descriptor on a command.
 * @param command Command receiving the option.
 * @param spec Option descriptor.
 * @return Whether registration succeeded.
 */
bool flags_command_add_option(flags_command_t *command, const flags_option_spec_t *spec);
/**
 * @brief Collects options visible to a command, including inherited options.
 * @param command Command whose option scope is requested.
 * @param options Receives a newly allocated array of borrowed descriptors.
 * @param count Receives the descriptor count.
 * @return Whether collection succeeded.
 * @note Child descriptors with the same canonical name override an inherited
 *       descriptor. Free the returned array with
 *       flags_command_free_options(); descriptor storage remains caller-owned.
 */
bool flags_command_collect_options(const flags_command_t *command, const flags_option_spec_t ***options, size_t *count);
/**
 * @brief Frees an array returned by flags_command_collect_options().
 * @param options Array to free, or NULL.
 * @note The descriptors referenced by the array are not freed.
 */
void flags_command_free_options(const flags_option_spec_t **options);
/**
 * @brief Sets the command callback invoked by the application.
 * @param command Command to configure.
 * @param handler Application callback.
 * @param user_data Opaque callback data.
 * @return Whether configuration succeeded.
 */
bool flags_command_set_handler(flags_command_t *command, int (*handler)(flags_context_t *context, void *user_data), void *user_data);
/**
 * @brief Marks a descendant as the root's default command.
 * @param root Root command.
 * @param command Default descendant.
 * @return Whether configuration succeeded.
 */
bool flags_command_set_default(flags_command_t *root, flags_command_t *command);
/**
 * @brief Selects a command implicitly when the named root option is present.
 * @param root Root command.
 * @param command Implicit command.
 * @param option_name Root option name.
 * @return Whether configuration succeeded.
 */
bool flags_command_set_implicit_for_option(flags_command_t *root, flags_command_t *command, const char *option_name);
/**
 * @brief Resolves a command name from argv.
 * @param root Root command.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param command_index Optional command index output.
 * @param used_default Optional default-selection output.
 * @return Resolved command, or NULL when unresolved.
 */
flags_command_t *flags_resolve_command(flags_command_t *root, int argc, char **argv, int *command_index, bool *used_default);
/**
 * @brief Resolves a command and returns all resolution metadata.
 * @param root Root command.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param resolution Resolution output.
 * @return Whether resolution succeeded.
 */
bool flags_resolve_command_ex(flags_command_t *root, int argc, char **argv, flags_command_resolution_t *resolution);

/**
 * @brief Rejects root-only options placed after a selected command.
 *
 * @param context Context whose diagnostic hook receives the failure.
 * @param root Root command whose option scopes are inspected.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param resolution Resolution previously returned by
 *        flags_resolve_command_ex().
 * @return `true` when option ordering is valid; `false` otherwise.
 * @note The callback receives a complete diagnostic sentence but libflags
 *       does not choose its severity, formatting, or output destination.
 */
bool flags_command_validate_order(flags_context_t *context, const flags_command_t *root, int argc, char **argv,
                                  const flags_command_resolution_t *resolution);
/**
 * @brief Returns the command's canonical name.
 * @param command Command to inspect.
 * @return Borrowed command name.
 */
const char *flags_command_name(const flags_command_t *command);
/**
 * @brief Returns the command's parent, or NULL for a root command.
 * @param command Command to inspect.
 * @return Borrowed parent command.
 */
flags_command_t *flags_command_parent(const flags_command_t *command);
