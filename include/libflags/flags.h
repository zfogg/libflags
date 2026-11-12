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

/** A storage type understood by libflags. */
typedef enum flags_value_type {
  FLAGS_TYPE_BOOL,        /**< A boolean value. */
  FLAGS_TYPE_INT64,       /**< A signed 64-bit integer. */
  FLAGS_TYPE_UINT64,      /**< An unsigned 64-bit integer. */
  FLAGS_TYPE_DOUBLE,      /**< A double-precision value. */
  FLAGS_TYPE_STRING,      /**< An owned string value. */
  FLAGS_TYPE_ENUM,        /**< An enum index. */
  FLAGS_TYPE_PATH,        /**< A filesystem path. */
  FLAGS_TYPE_FILE,        /**< A file path. */
  FLAGS_TYPE_DIRECTORY,   /**< A directory path. */
  FLAGS_TYPE_URL,         /**< A URL. */
  FLAGS_TYPE_HOST,        /**< A host name. */
  FLAGS_TYPE_ADDRESS,     /**< A host or network address. */
  FLAGS_TYPE_ENDPOINT,    /**< A host, port, and optional scheme. */
  FLAGS_TYPE_PORT,        /**< A network port. */
  FLAGS_TYPE_DURATION,    /**< A duration in nanoseconds. */
  FLAGS_TYPE_TIMESTAMP,   /**< A timestamp. */
  FLAGS_TYPE_SIZE,        /**< A byte size with optional suffix. */
  FLAGS_TYPE_RATE,        /**< A rate with optional suffix. */
  FLAGS_TYPE_PERCENT,     /**< A percentage value. */
  FLAGS_TYPE_DECIBEL,     /**< A decibel value or `auto`. */
  FLAGS_TYPE_STRING_LIST, /**< A list of strings. */
  FLAGS_TYPE_INT_LIST,    /**< A list of integers. */
  FLAGS_TYPE_ENUM_LIST,   /**< A list of enum values. */
  FLAGS_TYPE_COLOR,       /**< A color value. */
  FLAGS_TYPE_COLOR_MODE,  /**< A terminal color mode. */
  FLAGS_TYPE_LOG_LEVEL,   /**< A logging level. */
  FLAGS_TYPE_CUSTOM       /**< A callback-defined value. */
} flags_value_type_t;

/** The operation applied when a flag is repeated. */
typedef enum flags_operation {
  FLAGS_VALUE_ASSIGN,   /**< Replace the current value. */
  FLAGS_VALUE_TOGGLE,   /**< Toggle a boolean value. */
  FLAGS_VALUE_ADD,      /**< Add a numeric value. */
  FLAGS_VALUE_SUBTRACT, /**< Subtract a numeric value. */
  FLAGS_VALUE_APPEND,   /**< Append to a list or string. */
  FLAGS_VALUE_CYCLE     /**< Cycle through enum values. */
} flags_operation_t;

/** The behavior of a flag independent of its value type. */
typedef enum flags_option_behavior {
  FLAGS_BEHAVIOR_VALUE,   /**< Parse and store a value. */
  FLAGS_BEHAVIOR_ACTION,  /**< Invoke an action when present. */
  FLAGS_BEHAVIOR_CALLBACK /**< Invoke application callback behavior. */
} flags_option_behavior_t;

/** Whether a flag consumes a following command-line value. */
typedef enum flags_argument_requirement {
  FLAGS_ARGUMENT_NONE,     /**< The flag has no value. */
  FLAGS_ARGUMENT_REQUIRED, /**< The flag must have a value. */
  FLAGS_ARGUMENT_OPTIONAL  /**< The value may be omitted. */
} flags_argument_requirement_t;

/** The meaning of a single hyphen for a file-valued option. */
typedef enum flags_dash_behavior {
  FLAGS_DASH_NOT_ALLOWED,    /**< Reject `-`. */
  FLAGS_DASH_LITERAL,        /**< Preserve `-` as an ordinary file name. */
  FLAGS_DASH_STDIN,          /**< Classify `-` as standard input. */
  FLAGS_DASH_STDOUT,         /**< Classify `-` as standard output. */
  FLAGS_DASH_STDIN_OR_STDOUT /**< Permit either stream direction. */
} flags_dash_behavior_t;

/** The normalized stream classification of a string value. */
typedef enum flags_stream_kind {
  FLAGS_STREAM_NONE,  /**< No stream classification. */
  FLAGS_STREAM_FILE,  /**< A regular file or URL-like path. */
  FLAGS_STREAM_STDIN, /**< Standard input. */
  FLAGS_STREAM_STDOUT /**< Standard output. */
} flags_stream_kind_t;

/** The number of values accepted by a positional descriptor. */
typedef enum flags_positional_arity {
  FLAGS_POSITIONAL_OPTIONAL,     /**< Zero or one value. */
  FLAGS_POSITIONAL_REQUIRED,     /**< Exactly one value. */
  FLAGS_POSITIONAL_ZERO_OR_MORE, /**< Any number of values. */
  FLAGS_POSITIONAL_ONE_OR_MORE   /**< At least one value. */
} flags_positional_arity_t;

/** The source used when assigning an option. */
typedef enum flags_source {
  FLAGS_SOURCE_DEFAULT,      /**< A schema default. */
  FLAGS_SOURCE_CONFIG,       /**< A configuration document. */
  FLAGS_SOURCE_ENVIRONMENT,  /**< An environment variable. */
  FLAGS_SOURCE_COMMAND_LINE, /**< A command-line flag. */
  FLAGS_SOURCE_RUNTIME       /**< A live runtime update. */
} flags_source_t;

/** A logging severity emitted by a context. */
typedef enum flags_log_level { FLAGS_LOG_TRACE, FLAGS_LOG_DEBUG, FLAGS_LOG_INFO, FLAGS_LOG_WARNING, FLAGS_LOG_ERROR } flags_log_level_t;

/** A category for a diagnostic event. */
typedef enum flags_log_category { FLAGS_LOG_PARSE, FLAGS_LOG_VALIDATION, FLAGS_LOG_RUNTIME, FLAGS_LOG_JSON, FLAGS_LOG_CONFIG, FLAGS_LOG_COMMAND, FLAGS_LOG_MEMORY } flags_log_category_t;

/** A typed value passed to or copied from the library. */
typedef union flags_value {
  bool boolean;              /**< Boolean representation. */
  int64_t integer;           /**< Signed integer representation. */
  uint64_t unsigned_integer; /**< Unsigned integer representation. */
  double number;             /**< Floating-point representation. */
  const char *string;        /**< String representation. */
} flags_value_t;

/** An opaque, independently usable parser instance. */
typedef struct flags_context flags_context_t;

/** Parses a textual value into its typed representation. */
typedef bool (*flags_parse_callback_t)(flags_context_t *, const char *, flags_value_t *, void *);
/** Validates a typed value before it is stored. */
typedef bool (*flags_validate_callback_t)(const flags_context_t *, const char *, const flags_value_t *, char *, size_t, void *);
/** Observes a successful value change after storage is updated. */
typedef void (*flags_change_callback_t)(flags_context_t *, const char *, const flags_value_t *, const flags_value_t *, void *);

/** A diagnostic event delivered to the instance logging hook. */
typedef struct flags_log_event {
  flags_log_level_t level;       /**< Event severity. */
  flags_log_category_t category; /**< Event category. */
  const char *message;           /**< Borrowed event text valid during callback. */
  const char *option_name;       /**< Related option, if any. */
  const char *command_name;      /**< Related command, if any. */
  flags_source_t source;         /**< Origin of the operation. */
  uint64_t revision;             /**< Context revision at emission. */
} flags_log_event_t;

/** Receives diagnostic events from one context. */
typedef void (*flags_log_callback_t)(const flags_log_event_t *, void *);

/** Configures optional per-context diagnostics. */
typedef struct flags_logging_config {
  flags_log_callback_t callback;   /**< Callback, or NULL to disable logging. */
  void *user_data;                 /**< Callback user data. */
  flags_log_level_t minimum_level; /**< Lowest severity delivered. */
} flags_logging_config_t;

/** One named value in an atomic runtime update batch. */
typedef struct flags_update {
  const char *name;    /**< Option name. */
  flags_value_t value; /**< New typed value. */
} flags_update_t;

/** Configures automatic environment-variable lookup for one context. */
typedef struct flags_environment_config {
  const char *prefix; /**< Prefix for derived names, or NULL for none. */
  bool derive_names;  /**< Derive names from option names when enabled. */
} flags_environment_config_t;

/** A value and its normalized stream interpretation. */
typedef struct flags_stream_value {
  const char *text;         /**< The original, borrowed text. */
  flags_stream_kind_t kind; /**< The stream classification. */
} flags_stream_value_t;

/** Results of parsing the command-line portion of an argument vector. */
typedef struct flags_parse_result {
  bool options_ended;         /**< Whether an explicit `--` was consumed. */
  int positional_argc;        /**< Number of borrowed positional arguments. */
  char **positional_argv;     /**< Borrowed positional argument pointers. */
  int first_positional_index; /**< Index in the original argument vector. */
} flags_parse_result_t;

/** Parse one positional value into application-owned state. */
typedef bool (*flags_parse_positional_callback_t)(flags_context_t *, const char *, void *);

/** Describes one positional argument family. */
typedef struct flags_positional_spec {
  const char *name;                        /**< Stable descriptor name. */
  const char *metavar;                     /**< Help placeholder. */
  const char *description;                 /**< Help text. */
  flags_positional_arity_t arity;          /**< Accepted cardinality. */
  flags_parse_positional_callback_t parse; /**< Semantic parser callback. */
  void *user_data;                         /**< Callback user data. */
} flags_positional_spec_t;

/** Describes one option and its binding into application storage. */
typedef struct flags_option_spec {
  const char *name;                                  /**< Long name without hyphens. */
  const char *environment_name;                      /**< Explicit environment name, or NULL to derive it. */
  char short_name;                                   /**< Optional one-character name. */
  flags_value_type_t type;                           /**< Storage/value type. */
  flags_option_behavior_t behavior;                  /**< Value, action, or callback behavior. */
  flags_operation_t default_operation;               /**< Operation for repeated flags. */
  flags_argument_requirement_t argument_requirement; /**< Value cardinality. */
  flags_dash_behavior_t dash_behavior;               /**< Meaning of a value equal to `-`. */
  bool repeatable;                                   /**< Whether the flag may repeat. */
  size_t storage_offset;                             /**< Byte offset in application storage. */
  size_t storage_size;                               /**< Bound field size in bytes. */
  const void *default_value;                         /**< Optional default value. */
  const char *const *enum_values;                    /**< Optional enum spellings. */
  size_t enum_value_count;                           /**< Number of enum spellings. */
  bool dynamic_string;                               /**< Whether strings are duplicated. */
  flags_parse_callback_t parse;                      /**< Optional textual parser. */
  flags_validate_callback_t validate;                /**< Optional validator. */
  flags_change_callback_t on_change;                 /**< Optional change callback. */
  void *user_data;                                   /**< Callback context. */
} flags_option_spec_t;

/** Immutable collection of option and positional descriptors. */
typedef struct flags_schema {
  const flags_option_spec_t *options;         /**< Option descriptor array. */
  size_t option_count;                        /**< Number of options. */
  const flags_positional_spec_t *positionals; /**< Positional descriptor array. */
  size_t positional_count;                    /**< Number of positionals. */
} flags_schema_t;

/** Creates an independent context bound to caller-owned storage. */
flags_context_t *flags_create(const flags_schema_t *, void *, size_t);
/** Destroys a context without destroying caller-owned storage. */
void flags_destroy(flags_context_t *);
/** Copies schema defaults into bound storage. */
bool flags_set_defaults(flags_context_t *);
/** Parses flags and returns borrowed positional arguments. */
bool flags_parse(flags_context_t *, int, char **, flags_parse_result_t *);
/** Validates that a context has usable storage. */
bool flags_validate(const flags_context_t *);
/** Copies one option value into caller-provided storage. */
bool flags_get(const flags_context_t *, const char *, flags_value_t *);
/** Validates and assigns one option value. */
bool flags_set(flags_context_t *, const char *, const flags_value_t *, flags_source_t);
/** Frees a string returned by flags_get_string. */
void flags_free_string(char *);
/** Retrieves a duplicated string value. */
bool flags_get_string(const flags_context_t *, const char *, char **);
/** Classifies a string according to an option's dash metadata. */
bool flags_classify_stream(const flags_option_spec_t *, const char *, flags_stream_value_t *);
/** Reports whether an option was explicitly assigned. */
bool flags_was_explicitly_set(const flags_context_t *, const char *);
/** Returns the number of successful assignments to an option. */
uint64_t flags_change_count(const flags_context_t *, const char *);
/** Reports whether the current value differs from its default. */
bool flags_changed_from_default(const flags_context_t *, const char *);
/** Returns successful read count. */
uint64_t flags_read_count(const flags_context_t *, const char *);
/** Returns assignment-attempt count. */
uint64_t flags_write_count(const flags_context_t *, const char *);
/** Copies the last-change timestamp. */
bool flags_last_changed_at(const flags_context_t *, const char *, struct timespec *);
/** Returns elapsed nanoseconds since the last change. */
bool flags_time_since_change(const flags_context_t *, const char *, uint64_t *);
/** Installs or clears the context logging hook. */
bool flags_set_logging(flags_context_t *, const flags_logging_config_t *);
/** Emits a formatted diagnostic event when logging is enabled. */
void flags_log(flags_context_t *, flags_log_level_t, flags_log_category_t, const char *, const char *, ...);
/** Installs per-context automatic environment-variable configuration. */
bool flags_set_environment_config(flags_context_t *, const flags_environment_config_t *);
/** Returns the derived or explicit environment name for an option. */
bool flags_environment_name(const flags_context_t *, const char *, char *, size_t);
/** Applies present environment variables using environment precedence. */
bool flags_apply_environment(flags_context_t *);
/** Reads one value and returns the context revision observed with it. */
bool flags_runtime_read(const flags_context_t *, const char *, flags_value_t *, uint64_t *);
/** Applies one value if the expected revision still matches. */
bool flags_runtime_apply(flags_context_t *, const char *, const flags_value_t *, uint64_t, flags_source_t, uint64_t *);
/** Returns the current context revision. */
uint64_t flags_runtime_revision(const flags_context_t *);
/** Applies an all-or-nothing batch at one expected revision. */
bool flags_runtime_apply_batch(flags_context_t *, const flags_update_t *, size_t, uint64_t, flags_source_t, uint64_t *);
