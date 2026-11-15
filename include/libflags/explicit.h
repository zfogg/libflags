/**
 * @file explicit.h
 * @brief Persistent snapshots of libflags option source metadata.
 */
#pragma once

#include <stdbool.h>

#include <libflags/flags.h>

/** Opaque snapshot of which options have explicit assignments. */
typedef struct flags_explicit_snapshot flags_explicit_snapshot_t;

/**
 * @brief Creates an explicitness snapshot from a flags context.
 * @param context Context whose option source metadata is copied.
 * @return Owned snapshot, or NULL on invalid input/allocation failure.
 */
flags_explicit_snapshot_t *flags_explicit_snapshot_create(const flags_context_t *context);

/**
 * @brief Merges explicit assignments from a context into a snapshot.
 * @param snapshot Snapshot to update.
 * @param context Context whose explicitly assigned options are merged.
 * @return Whether the merge succeeded.
 */
bool flags_explicit_snapshot_merge(flags_explicit_snapshot_t *snapshot, const flags_context_t *context);

/**
 * @brief Checks whether an option is explicitly assigned in a snapshot.
 * @param snapshot Snapshot to inspect.
 * @param name Canonical option name.
 * @return true when the option was explicitly assigned, otherwise false.
 */
bool flags_explicit_snapshot_was_set(const flags_explicit_snapshot_t *snapshot, const char *name);

/**
 * @brief Destroys an explicitness snapshot.
 * @param snapshot Snapshot to release; NULL is allowed.
 */
void flags_explicit_snapshot_destroy(flags_explicit_snapshot_t *snapshot);

/**
 * @brief Merges explicit assignments into libflags' process-wide option state.
 * @param context Context whose source metadata is merged.
 * @return true on success, or false when the context is invalid/allocation fails.
 * @note This state survives destruction of short-lived parse/configuration contexts.
 */
bool flags_explicit_state_merge(const flags_context_t *context);

/**
 * @brief Checks libflags' process-wide explicit option state.
 * @param name Canonical option name.
 * @return true when the option was assigned by a non-default source.
 */
bool flags_explicit_state_was_set(const char *name);

/** @brief Clears libflags' process-wide explicit option state. */
void flags_explicit_state_destroy(void);
