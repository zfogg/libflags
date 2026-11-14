#include <libflags/log.h>

#include "internal/context.h"

bool flags_set_logging(flags_context_t *context, const flags_logging_config_t *config) {
  if (!context)
    return false;
  context->logging = config ? *config : (flags_logging_config_t){0};
  return true;
}
