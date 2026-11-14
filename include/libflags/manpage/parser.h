/**
 * @file parser.h
 * @brief Parse marked groff manpage sections.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/** @brief Classification of a parsed manpage section. */
typedef enum flags_manpage_section_type {
  FLAGS_MANPAGE_SECTION_UNMARKED,
  FLAGS_MANPAGE_SECTION_AUTO,
  FLAGS_MANPAGE_SECTION_MANUAL,
  FLAGS_MANPAGE_SECTION_MERGE,
} flags_manpage_section_type_t;

/** @brief A section extracted from a groff manpage template. */
typedef struct flags_manpage_section {
  /** Section name from the `.SH` directive, owned by this structure. */
  char *name;
  /** Section body, including its header, owned by this structure. */
  char *content;
  /** Number of bytes in @ref content, excluding its terminator. */
  size_t content_length;
  /** One-based source line where the section starts. */
  size_t start_line;
  /** One-based source line where the section ends. */
  size_t end_line;
  /** Marker classification for the section. */
  flags_manpage_section_type_t type;
  /** Whether a START/END marker enclosed the section. */
  bool has_markers;
} flags_manpage_section_t;

/**
 * @brief Parse sections from an open stream.
 * @param input Stream positioned at the beginning of the template.
 * @param[out] sections Receives an allocated section array.
 * @param[out] count Receives the number of sections.
 * @return `true` on success.
 * @note Free the result with flags_manpage_parser_free().
 */
bool flags_manpage_parser_parse(FILE *input, flags_manpage_section_t **sections, size_t *count);

/**
 * @brief Parse sections from a memory buffer.
 * @param content Template bytes.
 * @param length Number of bytes in @p content.
 * @param[out] sections Receives an allocated section array.
 * @param[out] count Receives the number of sections.
 * @return `true` on success.
 * @note The input does not need to be NUL terminated.
 */
bool flags_manpage_parser_parse_memory(const char *content, size_t length, flags_manpage_section_t **sections, size_t *count);

/**
 * @brief Free sections returned by the parser.
 * @param sections Section array, or `NULL`.
 * @param count Number of entries in @p sections.
 */
void flags_manpage_parser_free(flags_manpage_section_t *sections, size_t count);

/**
 * @brief Find a section by its exact name.
 * @param sections Section array.
 * @param count Number of entries in @p sections.
 * @param name Name to find.
 * @return Matching section, or `NULL`.
 */
const flags_manpage_section_t *flags_manpage_parser_find(const flags_manpage_section_t *sections, size_t count, const char *name);
