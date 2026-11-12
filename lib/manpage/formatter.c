#include <libflags/manpage/formatter.h>
#include <libplatform/system.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

const char *flags_manpage_escape_groff(const char *text) {
  return text ? text : "";
}

void flags_manpage_write_section(FILE *output, const char *section_name) {
  if (output && section_name)
    fprintf(output, ".SH %s\n", section_name);
}

void flags_manpage_write_blank_line(FILE *output) {
  if (output)
    fputc('\n', output);
}

void flags_manpage_write_bold(FILE *output, const char *text) {
  if (output && text)
    fprintf(output, ".B %s\n", text);
}

void flags_manpage_write_italic(FILE *output, const char *text) {
  if (output && text)
    fprintf(output, ".I %s\n", text);
}

void flags_manpage_write_tagged_paragraph(FILE *output) {
  if (output)
    fputs(".TP\n", output);
}

void flags_manpage_write_text(FILE *output, const char *text) {
  if (output)
    fprintf(output, "%s\n", text ? text : "");
}

void flags_manpage_write_title(FILE *output, const char *program_name, const char *mode_name, const char *description) {
  if (!output || !program_name || !description)
    return;
  time_t now = time(NULL);
  struct tm local_time;
  platform_localtime(&now, &local_time);
  char date[32];
  strftime(date, sizeof(date), "%B %Y", &local_time);
  char full_name[256];
  if (mode_name)
    snprintf(full_name, sizeof(full_name), "%s-%s", program_name, mode_name);
  else
    snprintf(full_name, sizeof(full_name), "%s", program_name);
  fprintf(output, ".TH %s 1 \"%s\" \"%s\" \"User Commands\"\n.SH NAME\n.B %s\n\\- %s\n\n", full_name, date, program_name, full_name, flags_manpage_escape_groff(description));
}
