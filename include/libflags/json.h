/** @file
 * @brief JSON integration boundary for libflags consumers.
 *
 * JSON is a transport/serialization format; libflags retains typed schema
 * storage as the primary model.
 */
#pragma once
#include <libflags/flags.h>

/** Result of applying one JSON patch document. */
typedef enum flags_patch_status { FLAGS_PATCH_APPLIED, FLAGS_PATCH_NOOP, FLAGS_PATCH_REVISION_CONFLICT, FLAGS_PATCH_INVALID } flags_patch_status_t;

/** Summary returned by flags_apply_json(). */
typedef struct flags_patch_result {
  flags_patch_status_t status;
  uint64_t revision;
  size_t changed_count;
} flags_patch_result_t;

/** Applies a JSON object as one typed, revision-checked update batch. */
bool flags_apply_json(flags_context_t *, const char *, size_t, uint64_t, flags_source_t, flags_patch_result_t *);

/** Serializes the current typed values into a caller-owned JSON string. */
bool flags_snapshot_json(const flags_context_t *, char **, uint64_t *);

/** Frees a string returned by flags_snapshot_json(). */
void flags_free_json(char *);
