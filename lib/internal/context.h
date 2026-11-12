#pragma once

#include <libflags/flags.h>
#include <libplatform/mutex.h>

typedef struct flags_stat {
  flags_source_t source;
  bool explicitly_set;
  bool changed_from_default;
  uint64_t change_count;
  uint64_t read_count;
  uint64_t write_count;
  struct timespec last_changed;
  struct timespec last_read;
} flags_stat_t;

struct flags_context {
  const flags_schema_t *schema;
  void *storage;
  size_t storage_size;
  flags_stat_t *stats;
  flags_logging_config_t logging;
  flags_environment_config_t environment;
  char *environment_prefix;
  uint64_t revision;
  bool preserve_storage;
  mutex_t mutex;
};
