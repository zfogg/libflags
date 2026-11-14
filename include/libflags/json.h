/** @file
 * @brief JSON integration boundary for libflags consumers.
 *
 * JSON is a transport/serialization format; libflags retains typed schema
 * storage as the primary model. JSON object keys are canonical option names;
 * values are converted through the same descriptors used by command-line,
 * environment, and TOML updates.
 */
#pragma once
#include <libflags/flags.h>

/** Result of applying one JSON patch document. */
typedef enum flags_patch_status {
  /** The patch was applied. */
  FLAGS_PATCH_APPLIED,
  /** The patch made no changes. */
  FLAGS_PATCH_NOOP,
  /** The expected revision did not match. */
  FLAGS_PATCH_REVISION_CONFLICT,
  /** The patch was invalid. */
  FLAGS_PATCH_INVALID
} flags_patch_status_t;

/** Summary returned by flags_apply_json(). */
typedef struct flags_patch_result {
  flags_patch_status_t status;
  uint64_t revision;
  size_t changed_count;
} flags_patch_result_t;

/**
 * Applies a JSON object as one typed, revision-checked update batch.
 *
 * The update is atomic: invalid input or a stale expected revision leaves the
 * context unchanged. Pass the revision returned by a previous snapshot, or
 * use the current revision when no optimistic concurrency check is needed.
 *
 * @param context Context whose bound storage will be updated.
 * @param json UTF-8 JSON object containing canonical option names.
 * @param json_size Number of bytes in @p json.
 * @param expected_revision Required context revision.
 * @param source Provenance recorded for each successful assignment.
 * @param result Optional result summary.
 * @return true when the JSON document was valid and applied or was a no-op.
 */
bool flags_apply_json(flags_context_t *context, const char *json, size_t length, uint64_t expected_revision, flags_source_t source, flags_patch_result_t *result);

/**
 * Serializes the current typed values into a caller-owned JSON string.
 *
 * The returned string must be released with flags_free_json().
 */
bool flags_snapshot_json(const flags_context_t *context, char **out_json, uint64_t *out_revision);

/** Frees a string returned by flags_snapshot_json(). */
void flags_free_json(char *json);
