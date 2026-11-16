#include <libflags/storage.h>
#include <libflags/values.h>

#include <string.h>
#include <stdlib.h>

static size_t primitive_size(flags_value_type_t type) {
  switch (type) {
  case FLAGS_TYPE_BOOL:
    return sizeof(bool);
  case FLAGS_TYPE_INT64:
    return sizeof(int32_t);
  case FLAGS_TYPE_UINT64:
    return sizeof(uint32_t);
  case FLAGS_TYPE_DOUBLE:
    return sizeof(float);
  case FLAGS_TYPE_STRING:
    return sizeof(char);
  default:
    return 0;
  }
}

bool flags_storage_is_set(flags_value_type_t type, const void *field, size_t field_size, const void *default_value, size_t default_size) {
  size_t size = primitive_size(type);
  if (!field || !size || field_size < size)
    return false;
  if (type == FLAGS_TYPE_STRING) {
    const char *value = field;
    const char *default_text = default_value && default_size >= sizeof(char *) ? *(const char *const *)default_value : NULL;
    return default_text ? strcmp(value, default_text) != 0 : value[0] != '\0';
  }
  unsigned char current[sizeof(uint64_t)] = {0};
  unsigned char initial[sizeof(uint64_t)] = {0};
  memcpy(current, field, size);
  if (default_value && default_size >= size)
    memcpy(initial, default_value, size);
  return memcmp(current, initial, size) != 0;
}

bool flags_storage_apply_text(flags_value_type_t type, void *field, size_t field_size, const char *text) {
  if (!field)
    return false;
  if (type == FLAGS_TYPE_BOOL) {
    bool value;
    if (!text) {
      if (field_size < sizeof(value))
        return false;
      memcpy(&value, field, sizeof(value));
      value = !value;
    } else if (!flags_parse_bool_relaxed(text, &value)) {
      return false;
    }
    if (field_size < sizeof(value))
      return false;
    memcpy(field, &value, sizeof(value));
    return true;
  }
  if (type == FLAGS_TYPE_DOUBLE) {
    double value;
    if (!flags_parse_double(text, &value) || field_size < sizeof(value))
      return false;
    memcpy(field, &value, sizeof(value));
    return true;
  }
  if (type == FLAGS_TYPE_INT64) {
    char *end = NULL;
    long long value = text ? strtoll(text, &end, 10) : 0;
    if (!text || end == text || *end != '\0' || field_size < sizeof(int))
      return false;
    int narrowed = (int)value;
    if ((long long)narrowed != value)
      return false;
    memcpy(field, &narrowed, sizeof(narrowed));
    return true;
  }
  if (type == FLAGS_TYPE_STRING) {
    if (!text || field_size == 0)
      return false;
    size_t length = strlen(text);
    if (length >= field_size)
      length = field_size - 1;
    memcpy(field, text, length);
    ((char *)field)[length] = '\0';
    return true;
  }
  return false;
}
