/** @file @brief Generic groff/troff formatting primitives. */
#pragma once

#include <stdio.h>

/** Returns a groff-safe borrowed string, or an empty string for NULL. */
const char *flags_manpage_escape_groff(const char *text);
/** Writes a groff section header. */
void flags_manpage_write_section(FILE *output, const char *section_name);
/** Writes a blank groff line. */
void flags_manpage_write_blank_line(FILE *output);
/** Writes bold groff text. */
void flags_manpage_write_bold(FILE *output, const char *text);
/** Writes italic groff text. */
void flags_manpage_write_italic(FILE *output, const char *text);
/** Writes a tagged paragraph marker. */
void flags_manpage_write_tagged_paragraph(FILE *output);
/** Writes a plain groff text line. */
void flags_manpage_write_text(FILE *output, const char *text);
/** Writes a standard user-command title block. */
void flags_manpage_write_title(FILE *output, const char *program_name, const char *mode_name, const char *description);
