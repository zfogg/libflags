#include <libflags/explicit.h>
#include <libplatform/util/string.h>

#include <stdlib.h>
#include <string.h>

typedef struct {
  char *name;
  bool explicitly_set;
} flags_explicit_entry_t;

struct flags_explicit_snapshot {
  flags_explicit_entry_t *entries;
  size_t count;
};

static size_t flags_explicit_find(const flags_explicit_snapshot_t *snapshot, const char *name) {
  if (!snapshot || !name)
    return SIZE_MAX;
  for (size_t i = 0; i < snapshot->count; ++i)
    if (strcmp(snapshot->entries[i].name, name) == 0)
      return i;
  return SIZE_MAX;
}

flags_explicit_snapshot_t *flags_explicit_snapshot_create(const flags_context_t *context) {
  const flags_schema_t *schema = flags_context_schema(context);
  if (!schema)
    return NULL;
  flags_explicit_snapshot_t *snapshot = calloc(1, sizeof(*snapshot));
  if (!snapshot)
    return NULL;
  snapshot->entries = calloc(schema->option_count, sizeof(*snapshot->entries));
  if (!snapshot->entries) {
    free(snapshot);
    return NULL;
  }
  snapshot->count = schema->option_count;
  for (size_t i = 0; i < snapshot->count; ++i) {
    snapshot->entries[i].name = platform_strdup(schema->options[i].name);
    if (!snapshot->entries[i].name) {
      flags_explicit_snapshot_destroy(snapshot);
      return NULL;
    }
  }
  if (!flags_explicit_snapshot_merge(snapshot, context)) {
    flags_explicit_snapshot_destroy(snapshot);
    return NULL;
  }
  return snapshot;
}

bool flags_explicit_snapshot_merge(flags_explicit_snapshot_t *snapshot, const flags_context_t *context) {
  const flags_schema_t *schema = flags_context_schema(context);
  if (!snapshot || !schema)
    return false;
  for (size_t i = 0; i < schema->option_count; ++i) {
    size_t index = flags_explicit_find(snapshot, schema->options[i].name);
    if (index != SIZE_MAX && flags_was_explicitly_set(context, schema->options[i].name))
      snapshot->entries[index].explicitly_set = true;
  }
  return true;
}

bool flags_explicit_snapshot_was_set(const flags_explicit_snapshot_t *snapshot, const char *name) {
  size_t index = flags_explicit_find(snapshot, name);
  return index != SIZE_MAX && snapshot->entries[index].explicitly_set;
}

void flags_explicit_snapshot_destroy(flags_explicit_snapshot_t *snapshot) {
  if (!snapshot)
    return;
  for (size_t i = 0; i < snapshot->count; ++i)
    free(snapshot->entries[i].name);
  free(snapshot->entries);
  free(snapshot);
}
