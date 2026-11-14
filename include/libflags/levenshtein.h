/** @file
 * @brief Suggestion-generation integration boundary.
 *
 * This API provides spelling suggestions used by help and diagnostics. It is
 * independent of schema and bound option storage.
 */
#pragma once
#include <libflags/flags.h>

/** @brief Maximum distance accepted by flags_levenshtein_find_similar(). */
#define FLAGS_LEVENSHTEIN_SUGGESTION_THRESHOLD 4

/**
 * @brief Computes the edit distance between two strings.
 * @param a First string.
 * @param b Second string.
 * @return Edit distance, or `SIZE_MAX` for invalid input.
 */
size_t flags_levenshtein(const char *a, const char *b);

/**
 * @brief Computes edit distance for explicit byte lengths.
 * @param a First string.
 * @param a_length First string length.
 * @param b Second string.
 * @param b_length Second string length.
 * @return Edit distance, or `SIZE_MAX` for invalid input.
 */
size_t flags_levenshtein_n(const char *a, size_t a_length, const char *b, size_t b_length);

/**
 * @brief Finds the closest candidate within the suggestion threshold.
 * @param unknown Value to match.
 * @param candidates NULL-terminated candidate list.
 * @return Borrowed best match, or NULL when no candidate is close enough.
 */
const char *flags_levenshtein_find_similar(const char *unknown, const char *const *candidates);
