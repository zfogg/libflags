#include <libflags/levenshtein.h>

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static size_t flags_utf8_length(const char *value) {
  size_t length = 0;
  const unsigned char *p = (const unsigned char *)value;
  while (*p) {
    size_t width = 1;
    if (*p >= 0xc2U && *p <= 0xdfU)
      width = 2;
    else if (*p >= 0xe0U && *p <= 0xefU)
      width = 3;
    else if (*p >= 0xf0U && *p <= 0xf4U)
      width = 4;
    else if (*p >= 0x80U)
      return SIZE_MAX;
    for (size_t i = 1; i < width; ++i)
      if ((p[i] & 0xc0U) != 0x80U)
        return SIZE_MAX;
    p += width;
    ++length;
  }
  return length;
}

size_t flags_levenshtein_n(const char *a, size_t a_length, const char *b, size_t b_length) {
  if (!a || !b)
    return SIZE_MAX;
  size_t *row = calloc(a_length + 1, sizeof(*row));
  if (!row)
    return SIZE_MAX;
  for (size_t i = 0; i <= a_length; ++i)
    row[i] = i;

  for (size_t j = 1; j <= b_length; ++j) {
    size_t previous = row[0];
    row[0] = j;
    for (size_t i = 1; i <= a_length; ++i) {
      const size_t saved = row[i];
      const size_t substitution = previous + (a[i - 1] != b[j - 1]);
      const size_t insertion = row[i - 1] + 1;
      const size_t deletion = row[i] + 1;
      row[i] = substitution < insertion ? (substitution < deletion ? substitution : deletion) : (insertion < deletion ? insertion : deletion);
      previous = saved;
    }
  }
  const size_t result = row[a_length];
  free(row);
  return result;
}

size_t flags_levenshtein(const char *a, const char *b) {
  if (!a || !b)
    return SIZE_MAX;

  const size_t a_length = flags_utf8_length(a);
  const size_t b_length = flags_utf8_length(b);
  if (a_length == SIZE_MAX || b_length == SIZE_MAX)
    return SIZE_MAX;

  uint32_t *a_points = calloc(a_length + 1, sizeof(*a_points));
  uint32_t *b_points = calloc(b_length + 1, sizeof(*b_points));
  if (!a_points || !b_points) {
    free(a_points);
    free(b_points);
    return SIZE_MAX;
  }

  const char *strings[] = {a, b};
  uint32_t *points[] = {a_points, b_points};
  const size_t lengths[] = {a_length, b_length};
  for (size_t string_index = 0; string_index < 2; ++string_index) {
    const unsigned char *p = (const unsigned char *)strings[string_index];
    for (size_t i = 0; i < lengths[string_index]; ++i) {
      uint32_t code = *p++;
      size_t continuation_count = (code >= 0xc0U) + (code >= 0xe0U) + (code >= 0xf0U);
      if ((code & 0xe0U) == 0xc0U)
        code &= 0x1fU;
      else if ((code & 0xf0U) == 0xe0U)
        code &= 0x0fU;
      else if ((code & 0xf8U) == 0xf0U)
        code &= 0x07U;
      for (size_t j = 0; j < continuation_count; ++j)
        code = (code << 6) | (*p++ & 0x3fU);
      points[string_index][i] = code;
    }
  }

  size_t *row = calloc(a_length + 1, sizeof(*row));
  if (!row) {
    free(a_points);
    free(b_points);
    return SIZE_MAX;
  }
  for (size_t i = 0; i <= a_length; ++i)
    row[i] = i;
  for (size_t j = 1; j <= b_length; ++j) {
    size_t previous = row[0];
    row[0] = j;
    for (size_t i = 1; i <= a_length; ++i) {
      const size_t saved = row[i];
      const size_t substitution = previous + (a_points[i - 1] != b_points[j - 1]);
      const size_t insertion = row[i - 1] + 1;
      const size_t deletion = row[i] + 1;
      row[i] = substitution < insertion ? (substitution < deletion ? substitution : deletion) : (insertion < deletion ? insertion : deletion);
      previous = saved;
    }
  }
  const size_t result = row[a_length];
  free(a_points);
  free(b_points);
  free(row);
  return result;
}

const char *flags_levenshtein_find_similar(const char *unknown, const char *const *candidates) {
  if (!unknown || !candidates)
    return NULL;
  const char *best = NULL;
  size_t best_distance = FLAGS_LEVENSHTEIN_SUGGESTION_THRESHOLD + 1;
  for (size_t i = 0; candidates[i]; ++i) {
    const size_t distance = flags_levenshtein(unknown, candidates[i]);
    if (distance < best_distance) {
      best = candidates[i];
      best_distance = distance;
    }
  }
  return best;
}
