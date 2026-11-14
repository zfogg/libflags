#include <libflags/json.h>

#include "internal/context.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <yyjson.h>

static const flags_option_spec_t *find_option(const flags_context_t *context, const char *name) {
  for (size_t i = 0; i < context->schema->option_count; ++i)
    if (!strcmp(context->schema->options[i].name, name))
      return &context->schema->options[i];
  return NULL;
}

static bool value_to_text(yyjson_val *value, char *buffer, size_t size) {
  if (yyjson_is_str(value)) {
    size_t length = yyjson_get_len(value);
    if (length >= size)
      return false;
    memcpy(buffer, yyjson_get_str(value), length);
    buffer[length] = '\0';
    return true;
  }
  if (yyjson_is_bool(value))
    return snprintf(buffer, size, "%s", yyjson_get_bool(value) ? "true" : "false") > 0;
  if (yyjson_is_num(value))
    return snprintf(buffer, size, "%.17g", yyjson_get_num(value)) > 0;
  return false;
}

bool flags_apply_json(flags_context_t *context, const char *json, size_t length, uint64_t expected_revision, flags_source_t source, flags_patch_result_t *result) {
  if (!context || !json || !result)
    return false;
  *result = (flags_patch_result_t){.status = FLAGS_PATCH_INVALID, .revision = flags_runtime_revision(context)};
  if (expected_revision != result->revision) {
    result->status = FLAGS_PATCH_REVISION_CONFLICT;
    return true;
  }
  yyjson_doc *document = yyjson_read_opts((char *)json, length, 0, NULL, NULL);
  yyjson_val *root = document ? yyjson_doc_get_root(document) : NULL;
  if (!root || !yyjson_is_obj(root)) {
    yyjson_doc_free(document);
    return true;
  }
  size_t count = yyjson_obj_size(root);
  flags_update_t *updates = count ? calloc(count, sizeof(*updates)) : NULL;
  char **texts = count ? calloc(count, sizeof(*texts)) : NULL;
  if (count && (!updates || !texts)) {
    free(updates);
    free(texts);
    yyjson_doc_free(document);
    return false;
  }
  bool valid = true;
  size_t index = 0;
  size_t max = yyjson_obj_size(root);
  yyjson_val *key, *value;
  yyjson_obj_foreach(root, index, max, key, value) {
    const flags_option_spec_t *option = find_option(context, yyjson_get_str(key));
    if (!option || option->behavior == FLAGS_BEHAVIOR_ACTION || index >= count) {
      valid = false;
      break;
    }
    texts[index] = malloc(4096);
    if (!texts[index] || !value_to_text(value, texts[index], 4096) || !flags_parse_text(context, option->name, texts[index], &updates[index].value)) {
      valid = false;
      break;
    }
    updates[index].name = option->name;
  }
  if (valid && index) {
    uint64_t revision = 0;
    valid = flags_runtime_apply_batch(context, updates, index, expected_revision, source, &revision);
    result->revision = revision;
  } else {
    result->revision = flags_runtime_revision(context);
  }
  result->changed_count = valid ? index : 0;
  result->status = valid ? (index ? FLAGS_PATCH_APPLIED : FLAGS_PATCH_NOOP) : FLAGS_PATCH_INVALID;
  for (size_t i = 0; i < count; ++i)
    free(texts ? texts[i] : NULL);
  free(texts);
  free(updates);
  yyjson_doc_free(document);
  return true;
}

bool flags_snapshot_json(const flags_context_t *context, char **out_json, uint64_t *out_revision) {
  if (!context || !out_json)
    return false;
  yyjson_mut_doc *document = yyjson_mut_doc_new(NULL);
  yyjson_mut_val *root = document ? yyjson_mut_obj(document) : NULL;
  if (!root) {
    yyjson_mut_doc_free(document);
    return false;
  }
  yyjson_mut_doc_set_root(document, root);
  for (size_t i = 0; i < context->schema->option_count; ++i) {
    const flags_option_spec_t *option = &context->schema->options[i];
    flags_value_t value;
    if (!flags_get(context, option->name, &value))
      continue;
    switch (option->type) {
    case FLAGS_TYPE_BOOL:
      yyjson_mut_obj_add_bool(document, root, option->name, value.boolean);
      break;
    case FLAGS_TYPE_INT64:
      yyjson_mut_obj_add_sint(document, root, option->name, value.integer);
      break;
    case FLAGS_TYPE_UINT64:
      yyjson_mut_obj_add_uint(document, root, option->name, value.unsigned_integer);
      break;
    case FLAGS_TYPE_DOUBLE:
      yyjson_mut_obj_add_real(document, root, option->name, value.number);
      break;
    case FLAGS_TYPE_DECIBEL:
      if (value.decibel.automatic)
        yyjson_mut_obj_add_str(document, root, option->name, "auto");
      else
        yyjson_mut_obj_add_real(document, root, option->name, value.decibel.value);
      break;
    case FLAGS_TYPE_PERCENT:
      if (value.percent.automatic)
        yyjson_mut_obj_add_str(document, root, option->name, "auto");
      else
        yyjson_mut_obj_add_real(document, root, option->name, value.percent.value);
      break;
    default:
      if (value.string)
        yyjson_mut_obj_add_str(document, root, option->name, value.string);
      break;
    }
  }
  size_t length = 0;
  char *json = yyjson_mut_write(document, 0, &length);
  yyjson_mut_doc_free(document);
  if (!json)
    return false;
  *out_json = json;
  if (out_revision)
    *out_revision = flags_runtime_revision(context);
  return true;
}

void flags_free_json(char *json) {
  free(json);
}

