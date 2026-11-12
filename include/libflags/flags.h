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

/** A storage type understood by libflags. */
typedef enum flags_value_type {
  FLAGS_TYPE_BOOL,  /**< A boolean value. */
  FLAGS_TYPE_INT64, /**< A signed 64-bit integer. */
  FLAGS_TYPE_STRING /**< A borrowed string value. */
} flags_value_type_t;

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

/** A typed value passed to or copied from the library. */
typedef union flags_value {
  bool boolean;       /**< Boolean representation. */
  int64_t integer;    /**< Signed integer representation. */
  const char *string; /**< String representation. */
} flags_value_t;

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

/** An opaque, independently usable parser instance. */
typedef struct flags_context flags_context_t;

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
  char short_name;                                   /**< Optional one-character name. */
  flags_value_type_t type;                           /**< Storage/value type. */
  flags_argument_requirement_t argument_requirement; /**< Value cardinality. */
  flags_dash_behavior_t dash_behavior;               /**< Meaning of a value equal to `-`. */
  bool repeatable;                                   /**< Whether the flag may repeat. */
  size_t storage_offset;                             /**< Byte offset in application storage. */
  size_t storage_size;                               /**< Bound field size in bytes. */
  const void *default_value;                         /**< Optional default value. */
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
/** Classifies a string according to an option's dash metadata. */
bool flags_classify_stream(const flags_option_spec_t *, const char *, flags_stream_value_t *);
/** Reports whether an option was explicitly assigned. */
bool flags_was_explicitly_set(const flags_context_t *, const char *);
/** Returns the number of successful assignments to an option. */
uint64_t flags_change_count(const flags_context_t *, const char *);
