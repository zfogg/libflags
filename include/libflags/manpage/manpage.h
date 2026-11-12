/** @file @brief Generic groff manpage generation. */
#pragma once
#include <stdio.h>
#include <libflags/flags.h>

typedef struct flags_manpage_metadata {
  const char *program_name;
  const char *section;
  const char *description;
} flags_manpage_metadata_t;
/** Writes a generic option section; application prose remains caller-owned. */
bool flags_manpage_write(const flags_schema_t *, const flags_manpage_metadata_t *, FILE *);
