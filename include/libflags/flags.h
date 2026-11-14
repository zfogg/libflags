/** @file
 * @brief Typed, instance-based command-line option parsing.
 *
 * Applications provide an immutable schema and their own storage. A context
 * owns parsing metadata and statistics; it never exposes mutable internal
 * storage through a getter. The parser treats `--` as control and `-` as data.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

/**
 * @brief Storage types understood by libflags.
 */
typedef enum flags_value_type {
  /** A boolean value. */
  FLAGS_TYPE_BOOL,
  /** A signed 64-bit integer. */
  FLAGS_TYPE_INT64,
  /** An unsigned 64-bit integer. */
  FLAGS_TYPE_UINT64,
  /** A double-precision value. */
  FLAGS_TYPE_DOUBLE,
  /** An owned string value. */
  FLAGS_TYPE_STRING,
  /** An enum index. */
  FLAGS_TYPE_ENUM,
  /** A filesystem path. */
  FLAGS_TYPE_PATH,
  /** A file path. */
  FLAGS_TYPE_FILE,
  /** A directory path. */
  FLAGS_TYPE_DIRECTORY,
  /** A URL. */
  FLAGS_TYPE_URL,
  /** A host name. */
  FLAGS_TYPE_HOST,
  /** A host or network address. */
  FLAGS_TYPE_ADDRESS,
  /** A host, port, and optional scheme. */
  FLAGS_TYPE_ENDPOINT,
  /** A network port. */
  FLAGS_TYPE_PORT,
  /** A duration in nanoseconds. */
  FLAGS_TYPE_DURATION,
  /** A timestamp. */
  FLAGS_TYPE_TIMESTAMP,
  /** A byte size with optional suffix. */
  FLAGS_TYPE_SIZE,
  /** A rate with optional suffix. */
  FLAGS_TYPE_RATE,
  /** A percentage value. */
  FLAGS_TYPE_PERCENT,
  /** A decibel value or `auto`. */
  FLAGS_TYPE_DECIBEL,
  /** A list of strings. */
  FLAGS_TYPE_STRING_LIST,
  /** A list of integers. */
  FLAGS_TYPE_INT_LIST,
  /** A list of enum values. */
  FLAGS_TYPE_ENUM_LIST,
  /** A color value. */
  FLAGS_TYPE_COLOR,
  /** A terminal color mode. */
  FLAGS_TYPE_COLOR_MODE,
  /** A logging level. */
  FLAGS_TYPE_LOG_LEVEL,
  /** A callback-defined value. */
  FLAGS_TYPE_CUSTOM
} flags_value_type_t;

/**
 * @brief Operations applied when a flag is repeated.
 */
typedef enum flags_operation {
  /** Replace the current value. */
  FLAGS_VALUE_ASSIGN,
  /** Toggle a boolean value. */
  FLAGS_VALUE_TOGGLE,
  /** Add a numeric value. */
  FLAGS_VALUE_ADD,
  /** Subtract a numeric value. */
  FLAGS_VALUE_SUBTRACT,
  /** Append to a list or string. */
  FLAGS_VALUE_APPEND,
  /** Cycle through enum values. */
  FLAGS_VALUE_CYCLE
} flags_operation_t;

/**
 * @brief Behavior of a flag independent of its value type.
 */
typedef enum flags_option_behavior {
  /** Parse and store a value. */
  FLAGS_BEHAVIOR_VALUE,
  /** Invoke an action when present. */
  FLAGS_BEHAVIOR_ACTION,
  /** Invoke application callback behavior. */
  FLAGS_BEHAVIOR_CALLBACK
} flags_option_behavior_t;

/**
 * @brief Whether a flag consumes a following command-line value.
 */
typedef enum flags_argument_requirement {
  /** The flag has no value. */
  FLAGS_ARGUMENT_NONE,
  /** The flag must have a value. */
  FLAGS_ARGUMENT_REQUIRED,
  /** The value may be omitted. */
  FLAGS_ARGUMENT_OPTIONAL
} flags_argument_requirement_t;

/**
 * @brief Meanings of a single hyphen for a file-valued option.
 */
typedef enum flags_dash_behavior {
  /** Reject `-`. */
  FLAGS_DASH_NOT_ALLOWED,
  /** Preserve `-` as an ordinary file name. */
  FLAGS_DASH_LITERAL,
  /** Classify `-` as standard input. */
  FLAGS_DASH_STDIN,
  /** Classify `-` as standard output. */
  FLAGS_DASH_STDOUT,
  /** Permit either stream direction. */
  FLAGS_DASH_STDIN_OR_STDOUT
} flags_dash_behavior_t;

/**
 * @brief Normalized stream classifications for string values.
 */
typedef enum flags_stream_kind {
  /** No stream classification. */
  FLAGS_STREAM_NONE,
  /** A regular file or URL-like path. */
  FLAGS_STREAM_FILE,
  /** Standard input. */
  FLAGS_STREAM_STDIN,
  /** Standard output. */
  FLAGS_STREAM_STDOUT
} flags_stream_kind_t;

/**
 * @brief Numbers of values accepted by a positional descriptor.
 */
typedef enum flags_positional_arity {
  /** Zero or one value. */
  FLAGS_POSITIONAL_OPTIONAL,
  /** Exactly one value. */
  FLAGS_POSITIONAL_REQUIRED,
  /** Any number of values. */
  FLAGS_POSITIONAL_ZERO_OR_MORE,
  /** At least one value. */
  FLAGS_POSITIONAL_ONE_OR_MORE
} flags_positional_arity_t;

/**
 * @brief Policies for positional values encountered before `--`.
 */
typedef enum flags_argument_order {
  /** Continue scanning for options. */
  FLAGS_POSITIONAL_PERMUTE,
  /** Stop option parsing at the first value. */
  FLAGS_POSITIONAL_STOP,
  /** Accept positionals only after `--`. */
  FLAGS_POSITIONAL_ONLY_AFTER_TERMINATOR
} flags_argument_order_t;

/**
 * @brief Sources recorded when assigning an option.
 */
typedef enum flags_source {
  /** A schema default. */
  FLAGS_SOURCE_DEFAULT,
  /** A configuration document. */
  FLAGS_SOURCE_CONFIG,
  /** An environment variable. */
  FLAGS_SOURCE_ENVIRONMENT,
  /** A command-line flag. */
  FLAGS_SOURCE_COMMAND_LINE,
  /** A live runtime update. */
  FLAGS_SOURCE_RUNTIME,
  /** An application callback update. */
  FLAGS_SOURCE_CALLBACK
} flags_source_t;

/**
 * @brief Numeric value that can preserve an explicit `auto` spelling.
 */
typedef struct flags_semantic_number {
  double value;
  bool automatic;
} flags_semantic_number_t;

typedef flags_semantic_number_t flags_decibel_t;
typedef flags_semantic_number_t flags_percent_t;

/**
 * @brief Typed value passed to or copied from the library.
 */
typedef union flags_value {
  /** Boolean representation. */
  bool boolean;
  /** Signed integer representation. */
  int64_t integer;
  /** Unsigned integer representation. */
  uint64_t unsigned_integer;
  /** Floating-point representation. */
  double number;
  /** String representation. */
  const char *string;
  /** Decibel value, including `auto`. */
  flags_decibel_t decibel;
  /** Percentage value, including `auto`. */
  flags_percent_t percent;
} flags_value_t;

/**
 * @brief Opaque, independently usable parser instance.
 */
typedef struct flags_context flags_context_t;

/** Parses a textual value into its typed representation. */
typedef bool (*flags_parse_callback_t)(flags_context_t *context, const char *text, flags_value_t *value, void *user_data);
/** Validates a typed value before it is stored. */
typedef bool (*flags_validate_callback_t)(const flags_context_t *context, const char *text, const flags_value_t *value, char *error, size_t error_size, void *user_data);
/**
 * @brief Observes a successful value change after storage is updated.
 * @param context Context that changed.
 * @param name Canonical option name.
 * @param old_value Previous value.
 * @param new_value New value.
 * @param user_data Callback-owned data.
 */
typedef void (*flags_change_callback_t)(flags_context_t *context, const char *name, const flags_value_t *old_value, const flags_value_t *new_value, void *user_data);

/**
 * @brief Diagnostic event delivered to the instance logging hook.
 * @note The consumer owns severity, filtering, formatting, and routing.
 */
typedef struct flags_log_event {
  /** Borrowed event text valid during callback. */
  const char *message;
  /** Related option, if any. */
  const char *option_name;
  /** Related command, if any. */
  const char *command_name;
  /** Origin of the operation. */
  flags_source_t source;
  /** Context revision at emission. */
  uint64_t revision;
} flags_log_event_t;

/**
 * @brief Receives diagnostic events from one context.
 * @param event Borrowed diagnostic event.
 * @param user_data Callback-owned data.
 */
typedef void (*flags_log_callback_t)(const flags_log_event_t *event, void *user_data);

/**
 * @brief Configures an optional per-context diagnostic callback.
 */
typedef struct flags_logging_config {
  /** Callback, or NULL to disable logging. */
  flags_log_callback_t callback;
  /** Callback user data. */
  void *user_data;
} flags_logging_config_t;

/**
 * @brief One named value in an atomic runtime update batch.
 */
typedef struct flags_update {
  /** Option name. */
  const char *name;
  /** New typed value. */
  flags_value_t value;
} flags_update_t;

/**
 * @brief Configures automatic environment-variable lookup for one context.
 */
typedef struct flags_environment_config {
  /** Prefix for derived names, or NULL for none. */
  const char *prefix;
  /** Derive names from option names when enabled. */
  bool derive_names;
} flags_environment_config_t;

/**
 * @brief Value and normalized stream interpretation.
 */
typedef struct flags_stream_value {
  /** The original, borrowed text. */
  const char *text;
  /** The stream classification. */
  flags_stream_kind_t kind;
} flags_stream_value_t;

/**
 * @brief Results of parsing the command-line portion of an argument vector.
 */
typedef struct flags_parse_result {
  /** Whether an explicit `--` was consumed. */
  bool options_ended;
  /** Number of borrowed positional arguments. */
  int positional_argc;
  /** Borrowed positional argument pointers. */
  char **positional_argv;
  /** Index in the original argument vector. */
  int first_positional_index;
} flags_parse_result_t;

/** Parses one positional value into application-owned state. */
typedef bool (*flags_parse_positional_callback_t)(flags_context_t *context, const char *text, void *user_data);

/**
 * @brief Describes one positional argument family.
 */
typedef struct flags_positional_spec {
  /** Stable descriptor name. */
  const char *name;
  /** Help placeholder. */
  const char *metavar;
  /** Help text. */
  const char *description;
  /** Accepted cardinality. */
  flags_positional_arity_t arity;
  /** Semantic parser callback. */
  flags_parse_positional_callback_t parse;
  /** Callback user data. */
  void *user_data;
} flags_positional_spec_t;

/**
 * @brief Describes one option and its binding into application storage.
 */
typedef struct flags_option_spec {
  /** Long name without hyphens. */
  const char *name;
  /** Alternate long names. */
  const char *const *aliases;
  /** Number of alternate names. */
  size_t alias_count;
  /** Value placeholder for help and completion. */
  const char *metavar;
  /** Application-supplied help text. */
  const char *description;
  /** Explicit environment name, or NULL to derive it. */
  const char *environment_name;
  /** Optional one-character name. */
  char short_name;
  /** Storage/value type. */
  flags_value_type_t type;
  /** Value, action, or callback behavior. */
  flags_option_behavior_t behavior;
  /** Operation for repeated flags. */
  flags_operation_t default_operation;
  /** Value cardinality. */
  flags_argument_requirement_t argument_requirement;
  /** Meaning of a value equal to `-`. */
  flags_dash_behavior_t dash_behavior;
  /** Whether the flag may repeat. */
  bool repeatable;
  /** Redact this option in diagnostics. */
  bool secret;
  /** Byte offset in application storage. */
  size_t storage_offset;
  /** Bound field size in bytes. */
  size_t storage_size;
  /** Optional default value. */
  const void *default_value;
  /** Optional enum spellings. */
  const char *const *enum_values;
  /** Number of enum spellings. */
  size_t enum_value_count;
  /** Optional PCRE2 pattern for string validation. */
  const char *validation_regex;
  /** PCRE2 compile options for validation_regex. */
  uint32_t validation_regex_options;
  /** Whether strings are duplicated. */
  bool dynamic_string;
  /** Optional textual parser. */
  flags_parse_callback_t parse;
  /** Optional validator. */
  flags_validate_callback_t validate;
  /** Optional change callback. */
  flags_change_callback_t on_change;
  /** Callback context. */
  void *user_data;
} flags_option_spec_t;

/**
 * @brief Immutable collection of option and positional descriptors.
 */
typedef struct flags_schema {
  /** Option descriptor array. */
  const flags_option_spec_t *options;
  /** Number of options. */
  size_t option_count;
  /** Positional descriptor array. */
  const flags_positional_spec_t *positionals;
  /** Number of positionals. */
  size_t positional_count;
  /** Positional/option ordering policy. */
  flags_argument_order_t argument_order;
} flags_schema_t;

/**
 * @brief Creates an independent context bound to caller-owned storage.
 * @param schema Immutable option schema.
 * @param storage Application storage.
 * @param storage_size Storage size in bytes.
 * @return New context, or NULL on invalid input or allocation failure.
 * @note The caller retains ownership of storage.
 */
flags_context_t *flags_create(const flags_schema_t *schema, void *storage, size_t storage_size);
/**
 * @brief Returns the immutable schema associated with a context.
 * @param context Context to inspect.
 * @return Borrowed schema pointer.
 */
const flags_schema_t *flags_context_schema(const flags_context_t *context);
/**
 * @brief Destroys a context without destroying caller-owned storage.
 * @param context Context to destroy.
 */
void flags_destroy(flags_context_t *context);
/**
 * @brief Destroys a context while leaving dynamic values in bound storage.
 * @param context Context to destroy.
 * @note Dynamic values remain owned by the caller.
 */
void flags_destroy_preserving_storage(flags_context_t *context);
/**
 * @brief Copies schema defaults into bound storage.
 * @param context Context to initialize.
 * @return Whether defaults were applied.
 */
bool flags_set_defaults(flags_context_t *context);
/**
 * @brief Parses flags and returns borrowed positional arguments.
 * @param context Context to update.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param result Parse result destination.
 * @return Whether parsing succeeded.
 * @note Positional pointers remain owned by argv.
 */
bool flags_parse(flags_context_t *context, int argc, char **argv, flags_parse_result_t *result);
/**
 * @brief Applies a positional-only schema to borrowed values.
 * @param context Context to update.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Whether parsing succeeded.
 */
bool flags_parse_positionals(flags_context_t *context, int argc, char **argv);
/**
 * @brief Splits options from positional arguments.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param result Parse result destination.
 * @return Whether splitting succeeded.
 */
bool flags_split_arguments(int argc, char **argv, flags_parse_result_t *result);
/**
 * @brief Validates that a context has usable storage.
 * @param context Context to validate.
 * @return Whether the context is valid.
 */
bool flags_validate(const flags_context_t *context);
/**
 * @brief Copies one option value into caller-provided storage.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @param value Destination value.
 * @return Whether the option was found and copied.
 */
bool flags_get(const flags_context_t *context, const char *name, flags_value_t *value);
/**
 * @brief Validates and assigns one option value.
 * @param context Context to update.
 * @param name Canonical option name.
 * @param value Value to assign.
 * @param source Assignment provenance.
 * @return Whether assignment succeeded.
 */
bool flags_set(flags_context_t *context, const char *name, const flags_value_t *value, flags_source_t source);
/**
 * @brief Parses and assigns one textual option value.
 * @param context Context to update.
 * @param name Canonical option name.
 * @param text Textual value.
 * @param source Assignment provenance.
 * @return Whether assignment succeeded.
 */
bool flags_set_text(flags_context_t *context, const char *name, const char *text, flags_source_t source);
/**
 * @brief Parses one textual option value without changing context storage.
 * @param context Context providing the descriptor.
 * @param name Canonical option name.
 * @param text Textual value.
 * @param value Destination value.
 * @return Whether parsing succeeded.
 */
bool flags_parse_text(const flags_context_t *context, const char *name, const char *text, flags_value_t *value);
/**
 * @brief Retrieves a duplicated string value.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @param value Destination string pointer.
 * @return Whether the option was found and duplicated.
 * @note Release the returned string with free().
 */
bool flags_get_string(const flags_context_t *context, const char *name, char **value);
/**
 * @brief Classifies a string according to an option's dash metadata.
 * @param option Option descriptor.
 * @param text Text to classify.
 * @param stream Destination classification.
 * @return Whether classification succeeded.
 */
bool flags_classify_stream(const flags_option_spec_t *option, const char *text, flags_stream_value_t *stream);
/**
 * @brief Reports whether an option was explicitly assigned.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @return Whether the option has an explicit assignment.
 */
bool flags_was_explicitly_set(const flags_context_t *context, const char *name);
/**
 * @brief Returns the number of successful assignments to an option.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @return Assignment count, or zero for an unknown option.
 */
uint64_t flags_change_count(const flags_context_t *context, const char *name);
/**
 * @brief Reports whether the current value differs from its default.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @return Whether the value differs from its default.
 */
bool flags_changed_from_default(const flags_context_t *context, const char *name);
/**
 * @brief Returns the successful read count.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @return Read count, or zero for an unknown option.
 */
uint64_t flags_read_count(const flags_context_t *context, const char *name);
/**
 * @brief Returns the assignment-attempt count.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @return Write count, or zero for an unknown option.
 */
uint64_t flags_write_count(const flags_context_t *context, const char *name);
/**
 * @brief Copies the last-change timestamp.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @param timestamp Destination timestamp.
 * @return Whether a timestamp exists for the option.
 */
bool flags_last_changed_at(const flags_context_t *context, const char *name, struct timespec *timestamp);
/**
 * @brief Copies the last-read timestamp.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @param timestamp Destination timestamp.
 * @return Whether a timestamp exists for the option.
 */
bool flags_last_read_at(const flags_context_t *context, const char *name, struct timespec *timestamp);
/**
 * @brief Returns elapsed nanoseconds since the last change.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @param nanoseconds Destination elapsed time.
 * @return Whether the option has a recorded change.
 */
bool flags_time_since_change(const flags_context_t *context, const char *name, uint64_t *nanoseconds);
/**
 * @brief Installs or clears the context logging hook.
 * @param context Context to configure.
 * @param config Logging configuration, or NULL to clear it.
 * @return Whether the configuration was accepted.
 */
bool flags_set_logging(flags_context_t *context, const flags_logging_config_t *config);
/**
 * @brief Installs per-context automatic environment-variable configuration.
 * @param context Context to configure.
 * @param config Environment configuration, or NULL to clear it.
 * @return Whether configuration succeeded.
 */
bool flags_set_environment_config(flags_context_t *context, const flags_environment_config_t *config);
/**
 * @brief Returns the derived or explicit environment name for an option.
 * @param context Context containing the option.
 * @param option_name Canonical option name.
 * @param buffer Destination buffer.
 * @param buffer_size Destination capacity.
 * @return Whether the name fit in the buffer.
 */
bool flags_environment_name(const flags_context_t *context, const char *option_name, char *buffer, size_t buffer_size);
/**
 * @brief Derives an environment name from a prefix and canonical option name.
 * @param prefix Configured prefix, or NULL.
 * @param option_name Canonical option name.
 * @param buffer Destination buffer.
 * @param buffer_size Destination capacity.
 * @return Whether the name fit in the buffer.
 */
bool flags_derive_environment_name(const char *prefix, const char *option_name, char *buffer, size_t buffer_size);
/**
 * @brief Applies present environment variables using environment precedence.
 * @param context Context to update.
 * @return Whether all present variables were applied successfully.
 */
bool flags_apply_environment(flags_context_t *context);
/**
 * @brief Reads one value and returns the observed context revision.
 * @param context Context to inspect.
 * @param name Canonical option name.
 * @param value Destination value.
 * @param revision Destination revision.
 * @return Whether the option was found.
 */
bool flags_runtime_read(const flags_context_t *context, const char *name, flags_value_t *value, uint64_t *revision);
/**
 * @brief Applies one value if the expected revision still matches.
 * @param context Context to update.
 * @param name Canonical option name.
 * @param value Value to assign.
 * @param expected_revision Required current revision.
 * @param source Assignment provenance.
 * @param revision Destination resulting revision.
 * @return Whether the update was committed.
 */
bool flags_runtime_apply(flags_context_t *context, const char *name, const flags_value_t *value, uint64_t expected_revision, flags_source_t source, uint64_t *revision);
/**
 * @brief Returns the current context revision.
 * @param context Context to inspect.
 * @return Current revision, or zero for NULL.
 */
uint64_t flags_runtime_revision(const flags_context_t *context);
/**
 * @brief Applies an all-or-nothing batch at one expected revision.
 * @param context Context to update.
 * @param updates Update descriptors.
 * @param update_count Number of descriptors.
 * @param expected_revision Required current revision.
 * @param source Assignment provenance.
 * @param revision Destination resulting revision.
 * @return Whether every update was committed.
 */
bool flags_runtime_apply_batch(flags_context_t *context, const flags_update_t *updates, size_t update_count, uint64_t expected_revision, flags_source_t source, uint64_t *revision);
