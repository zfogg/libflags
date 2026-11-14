#include <libflags/levenshtein.h>

#include <stdlib.h>
#include <string.h>

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
      row[i] = substitution < insertion ? (substitution < deletion ? substitution : deletion)
                                        : (insertion < deletion ? insertion : deletion);
      previous = saved;
    }
  }
  const size_t result = row[a_length];
  free(row);
  return result;
}

size_t flags_levenshtein(const char *a, const char *b) {
  return a && b ? flags_levenshtein_n(a, strlen(a), b, strlen(b)) : SIZE_MAX;
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
