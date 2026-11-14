/** @file
 * @brief Generic groff/troff formatting primitives.
 */
#pragma once

#include <stdio.h>

/**
 * @brief Returns a groff-safe borrowed string.
 * @param text Text to escape, or NULL.
 * @return Borrowed escaped text, or an empty string.
 */
const char *flags_manpage_escape_groff(const char *text);
/** @brief Writes a groff section header.
 * @param output Destination stream.
 * @param section_name Section name.
 */
void flags_manpage_write_section(FILE *output, const char *section_name);
/**
 * @brief Writes a blank groff line.
 * @param output Destination stream.
 */
void flags_manpage_write_blank_line(FILE *output);
/**
 * @brief Writes bold groff text.
 * @param output Destination stream.
 * @param text Text to write.
 */
void flags_manpage_write_bold(FILE *output, const char *text);
/**
 * @brief Writes italic groff text.
 * @param output Destination stream.
 * @param text Text to write.
 */
void flags_manpage_write_italic(FILE *output, const char *text);
/**
 * @brief Writes a tagged paragraph marker.
 * @param output Destination stream.
 */
void flags_manpage_write_tagged_paragraph(FILE *output);
/**
 * @brief Writes a plain groff text line.
 * @param output Destination stream.
 * @param text Text to write.
 */
void flags_manpage_write_text(FILE *output, const char *text);
/** @brief Writes a standard user-command title block.
 * @param output Destination stream.
 * @param program_name Program name.
 * @param mode_name Mode or section name.
 * @param description Program description.
 */
void flags_manpage_write_title(FILE *output, const char *program_name, const char *mode_name, const char *description);
