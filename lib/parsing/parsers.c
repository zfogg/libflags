#include <libflags/values.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char *trimmed_duplicate(const char *start, size_t length) {
  while (length && isspace((unsigned char)*start))
    ++start, --length;
  while (length && isspace((unsigned char)start[length - 1]))
    --length;
  if (!length)
    return NULL;
  char *copy = malloc(length + 1);
  if (!copy)
    return NULL;
  memcpy(copy, start, length);
  copy[length] = '\0';
  return copy;
}

void flags_free_string_list(flags_string_list_t *list) {
  if (!list)
    return;
  for (size_t i = 0; i < list->count; ++i)
    free(list->items[i]);
  free(list->items);
  *list = (flags_string_list_t){0};
}

bool flags_parse_string_list(const char *text, flags_string_list_t *out) {
  if (!text || !out || !*text)
    return false;
  *out = (flags_string_list_t){0};
  const char *cursor = text;
  for (;;) {
    const char *separator = strchr(cursor, ',');
    size_t length = separator ? (size_t)(separator - cursor) : strlen(cursor);
    char *item = trimmed_duplicate(cursor, length);
    if (!item) {
      flags_free_string_list(out);
      return false;
    }
    char **items = realloc(out->items, (out->count + 1) * sizeof(*items));
    if (!items) {
      free(item);
      flags_free_string_list(out);
      return false;
    }
    out->items = items;
    out->items[out->count++] = item;
    if (!separator)
      return true;
    cursor = separator + 1;
  }
}

void flags_free_int_list(flags_int_list_t *list) {
  if (!list)
    return;
  free(list->items);
  *list = (flags_int_list_t){0};
}

bool flags_parse_int_list(const char *text, flags_int_list_t *out) {
  flags_string_list_t strings;
  if (!out || !flags_parse_string_list(text, &strings))
    return false;
  *out = (flags_int_list_t){0};
  out->items = calloc(strings.count, sizeof(*out->items));
  if (!out->items) {
    flags_free_string_list(&strings);
    return false;
  }
  for (size_t i = 0; i < strings.count; ++i) {
    char *end = NULL;
    out->items[i] = strtoll(strings.items[i], &end, 10);
    if (end == strings.items[i] || *end != '\0') {
      flags_free_string_list(&strings);
      flags_free_int_list(out);
      return false;
    }
  }
  out->count = strings.count;
  flags_free_string_list(&strings);
  return true;
}

static bool equal_case_insensitive(const char *left, const char *right) {
  while (*left && *right && tolower((unsigned char)*left) == tolower((unsigned char)*right)) {
    ++left;
    ++right;
  }
  return *left == '\0' && *right == '\0';
}

bool flags_parse_bool_relaxed(const char *text, bool *out) {
  if (!text || !out)
    return false;
  if (equal_case_insensitive(text, "true") || equal_case_insensitive(text, "yes") || equal_case_insensitive(text, "on") || !strcmp(text, "1")) {
    *out = true;
    return true;
  }
  if (equal_case_insensitive(text, "false") || equal_case_insensitive(text, "no") || equal_case_insensitive(text, "off") || !strcmp(text, "0")) {
    *out = false;
    return true;
  }
  return false;
}

static bool parse_number(const char *text, double *number, const char **suffix) {
  char *end = NULL;
  *number = strtod(text, &end);
  if (end == text)
    return false;
  *suffix = end;
  return true;
}

bool flags_parse_duration(const char *text, uint64_t *out) {
  if (!text || !out)
    return false;
  double number;
  const char *suffix;
  if (!parse_number(text, &number, &suffix) || number < 0.0)
    return false;
  uint64_t multiplier = 1000000000ULL;
  if (!strcmp(suffix, "ms"))
    multiplier = 1000000ULL;
  else if (!strcmp(suffix, "us"))
    multiplier = 1000ULL;
  else if (!strcmp(suffix, "ns"))
    multiplier = 1ULL;
  else if (!strcmp(suffix, "m"))
    multiplier = 60ULL * 1000000000ULL;
  else if (!strcmp(suffix, "h"))
    multiplier = 3600ULL * 1000000000ULL;
  else if (*suffix != '\0' && strcmp(suffix, "s"))
    return false;
  *out = (uint64_t)(number * (double)multiplier);
  return true;
}

bool flags_parse_size(const char *text, uint64_t *out) {
  if (!text || !out)
    return false;
  double number;
  const char *suffix;
  if (!parse_number(text, &number, &suffix) || number < 0.0)
    return false;
  uint64_t multiplier = 1;
  if (equal_case_insensitive(suffix, "k") || equal_case_insensitive(suffix, "kb"))
    multiplier = 1000ULL;
  else if (equal_case_insensitive(suffix, "kiB"))
    multiplier = 1024ULL;
  else if (equal_case_insensitive(suffix, "m") || equal_case_insensitive(suffix, "mb"))
    multiplier = 1000ULL * 1000ULL;
  else if (equal_case_insensitive(suffix, "miB"))
    multiplier = 1024ULL * 1024ULL;
  else if (equal_case_insensitive(suffix, "g") || equal_case_insensitive(suffix, "gb"))
    multiplier = 1000ULL * 1000ULL * 1000ULL;
  else if (equal_case_insensitive(suffix, "giB"))
    multiplier = 1024ULL * 1024ULL * 1024ULL;
  else if (*suffix != '\0')
    return false;
  *out = (uint64_t)(number * (double)multiplier);
  return true;
}

bool flags_parse_enum(const char *text, const char *const *values, size_t count, size_t *out) {
  if (!text || !values || !out)
    return false;
  for (size_t i = 0; i < count; ++i) {
    if (!strcmp(text, values[i])) {
      *out = i;
      return true;
    }
  }
  return false;
}

bool flags_parse_decibel_or_auto(const char *text, flags_decibel_t *out) {
  if (!text || !out)
    return false;
  if (equal_case_insensitive(text, "auto")) {
    out->value = 0.0;
    out->automatic = true;
    return true;
  }
  double number;
  const char *suffix;
  if (!parse_number(text, &number, &suffix) || (strcmp(suffix, "") && strcmp(suffix, "dB")))
    return false;
  out->value = number;
  out->automatic = false;
  return true;
}

bool flags_parse_percent_or_auto(const char *text, flags_percent_t *out) {
  if (!text || !out)
    return false;
  if (equal_case_insensitive(text, "auto")) {
    out->value = 0.0;
    out->automatic = true;
    return true;
  }
  char *end = NULL;
  double value = strtod(text, &end);
  if (end == text || *end != '\0' || value < 0.0 || value > 100.0)
    return false;
  out->value = value;
  out->automatic = false;
  return true;
}
