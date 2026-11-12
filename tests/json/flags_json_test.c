#include <libflags/json.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef struct json_storage {
  bool enabled;
  int64_t count;
  char *name;
} json_storage_t;

int main(void) {
  static const flags_option_spec_t options[] = {
      {.name = "enabled", .type = FLAGS_TYPE_BOOL, .storage_offset = offsetof(json_storage_t, enabled), .storage_size = sizeof(bool)},
      {.name = "count", .type = FLAGS_TYPE_INT64, .storage_offset = offsetof(json_storage_t, count), .storage_size = sizeof(int64_t)},
      {.name = "name", .type = FLAGS_TYPE_STRING, .storage_offset = offsetof(json_storage_t, name), .storage_size = sizeof(char *), .dynamic_string = true},
  };
  const flags_schema_t schema = {.options = options, .option_count = sizeof(options) / sizeof(*options)};
  json_storage_t storage = {0};
  flags_context_t *context = flags_create(&schema, &storage, sizeof(storage));
  assert(context);
  assert(flags_set_defaults(context));

  flags_patch_result_t result;
  const char json[] = "{\"enabled\":true,\"count\":4,\"name\":\"sample\"}";
  assert(flags_apply_json(context, json, sizeof(json) - 1, 0, FLAGS_SOURCE_RUNTIME, &result));
  assert(result.status == FLAGS_PATCH_APPLIED);
  assert(result.changed_count == 3);
  assert(result.revision == 1);
  assert(storage.enabled && storage.count == 4 && storage.name && !strcmp(storage.name, "sample"));

  char *snapshot = NULL;
  uint64_t revision = 0;
  assert(flags_snapshot_json(context, &snapshot, &revision));
  assert(snapshot && revision == 1);
  assert(strstr(snapshot, "\"enabled\":true"));
  assert(strstr(snapshot, "\"count\":4"));
  assert(strstr(snapshot, "\"name\":\"sample\""));
  flags_free_json(snapshot);
  flags_destroy(context);
  return 0;
}
