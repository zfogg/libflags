#include <libflags/format.h>

#include <stdio.h>

const char *flags_type_placeholder(flags_value_type_t type) {
  switch (type) {
  case FLAGS_TYPE_BOOL:
    return "BOOLEAN";
  case FLAGS_TYPE_INT64:
  case FLAGS_TYPE_UINT64:
  case FLAGS_TYPE_PORT:
    return "INTEGER";
  case FLAGS_TYPE_DOUBLE:
  case FLAGS_TYPE_PERCENT:
  case FLAGS_TYPE_DECIBEL:
    return "NUMBER";
  case FLAGS_TYPE_STRING:
  case FLAGS_TYPE_PATH:
  case FLAGS_TYPE_FILE:
  case FLAGS_TYPE_DIRECTORY:
  case FLAGS_TYPE_URL:
  case FLAGS_TYPE_HOST:
  case FLAGS_TYPE_ADDRESS:
  case FLAGS_TYPE_ENDPOINT:
    return "STRING";
  default:
    return "VALUE";
  }
}

int flags_format_default_value(flags_value_type_t type, const void *default_value, char *buffer, size_t buffer_size) {
  if (!default_value || !buffer || !buffer_size)
    return 0;
  switch (type) {
  case FLAGS_TYPE_BOOL:
    return snprintf(buffer, buffer_size, "%s", *(const bool *)default_value ? "true" : "false");
  case FLAGS_TYPE_INT64:
  case FLAGS_TYPE_PORT:
    return snprintf(buffer, buffer_size, "%lld", (long long)*(const int64_t *)default_value);
  case FLAGS_TYPE_UINT64:
  case FLAGS_TYPE_DURATION:
  case FLAGS_TYPE_SIZE:
  case FLAGS_TYPE_RATE:
    return snprintf(buffer, buffer_size, "%llu", (unsigned long long)*(const uint64_t *)default_value);
  case FLAGS_TYPE_DOUBLE:
    return snprintf(buffer, buffer_size, "%.2f", *(const double *)default_value);
  case FLAGS_TYPE_STRING:
  case FLAGS_TYPE_PATH:
  case FLAGS_TYPE_FILE:
  case FLAGS_TYPE_DIRECTORY:
  case FLAGS_TYPE_URL:
  case FLAGS_TYPE_HOST:
  case FLAGS_TYPE_ADDRESS:
  case FLAGS_TYPE_ENDPOINT:
    return snprintf(buffer, buffer_size, "%s", (const char *)default_value);
  default:
    return 0;
  }
}
