#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum { FLAGS_TYPE_BOOL, FLAGS_TYPE_INT64, FLAGS_TYPE_STRING } flags_value_type_t;
typedef enum { FLAGS_ARGUMENT_NONE, FLAGS_ARGUMENT_REQUIRED, FLAGS_ARGUMENT_OPTIONAL } flags_argument_requirement_t;
typedef enum { FLAGS_DASH_NOT_ALLOWED, FLAGS_DASH_LITERAL, FLAGS_DASH_STDIN, FLAGS_DASH_STDOUT, FLAGS_DASH_STDIN_OR_STDOUT } flags_dash_behavior_t;
typedef enum { FLAGS_STREAM_NONE, FLAGS_STREAM_FILE, FLAGS_STREAM_STDIN, FLAGS_STREAM_STDOUT } flags_stream_kind_t;
typedef enum { FLAGS_POSITIONAL_OPTIONAL, FLAGS_POSITIONAL_REQUIRED, FLAGS_POSITIONAL_ZERO_OR_MORE, FLAGS_POSITIONAL_ONE_OR_MORE } flags_positional_arity_t;
typedef enum { FLAGS_SOURCE_DEFAULT, FLAGS_SOURCE_CONFIG, FLAGS_SOURCE_ENVIRONMENT, FLAGS_SOURCE_COMMAND_LINE, FLAGS_SOURCE_RUNTIME } flags_source_t;
typedef union { bool boolean; int64_t integer; const char *string; } flags_value_t;
typedef struct { const char *text; flags_stream_kind_t kind; } flags_stream_value_t;
typedef struct { bool options_ended; int positional_argc; char **positional_argv; int first_positional_index; } flags_parse_result_t;
typedef struct flags_context flags_context_t;
typedef bool (*flags_parse_positional_callback_t)(flags_context_t *, const char *, void *);
typedef struct { const char *name; const char *metavar; const char *description; flags_positional_arity_t arity; flags_parse_positional_callback_t parse; void *user_data; } flags_positional_spec_t;
typedef struct { const char *name; char short_name; flags_value_type_t type; flags_argument_requirement_t argument_requirement; flags_dash_behavior_t dash_behavior; bool repeatable; size_t storage_offset; size_t storage_size; const void *default_value; } flags_option_spec_t;
typedef struct { const flags_option_spec_t *options; size_t option_count; const flags_positional_spec_t *positionals; size_t positional_count; } flags_schema_t;
flags_context_t *flags_create(const flags_schema_t *, void *, size_t);
void flags_destroy(flags_context_t *);
bool flags_set_defaults(flags_context_t *);
bool flags_parse(flags_context_t *, int, char **, flags_parse_result_t *);
bool flags_validate(const flags_context_t *);
bool flags_get(const flags_context_t *, const char *, flags_value_t *);
bool flags_set(flags_context_t *, const char *, const flags_value_t *, flags_source_t);
bool flags_classify_stream(const flags_option_spec_t *, const char *, flags_stream_value_t *);
bool flags_was_explicitly_set(const flags_context_t *, const char *);
uint64_t flags_change_count(const flags_context_t *, const char *);

