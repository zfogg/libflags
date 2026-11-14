#include <libflags/manpage/parser.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char *copy_trimmed(const char *begin, const char *end) {
  while (begin < end && isspace((unsigned char)*begin))
    ++begin;
  while (end > begin && isspace((unsigned char)end[-1]))
    --end;
  size_t length = (size_t)(end - begin);
  char *copy = malloc(length + 1);
  if (!copy)
    return NULL;
  memcpy(copy, begin, length);
  copy[length] = '\0';
  return copy;
}

static flags_manpage_section_type_t marker_type(const char *line) {
  const char *p = line;
  while (isspace((unsigned char)*p))
    ++p;
  if (strncmp(p, ".\\\"", 3) == 0)
    p += 3;
  else if (strncmp(p, ".\"", 2) == 0)
    p += 2;
  else
    return FLAGS_MANPAGE_SECTION_UNMARKED;
  while (isspace((unsigned char)*p))
    ++p;
  static const struct {
    const char *prefix;
    size_t length;
    flags_manpage_section_type_t type;
  } markers[] = {{"AUTO-START:", 11, FLAGS_MANPAGE_SECTION_AUTO}, {"AUTO-END:", 9, FLAGS_MANPAGE_SECTION_AUTO},
                 {"MANUAL-START:", 13, FLAGS_MANPAGE_SECTION_MANUAL}, {"MANUAL-END:", 11, FLAGS_MANPAGE_SECTION_MANUAL},
                 {"MERGE-START:", 12, FLAGS_MANPAGE_SECTION_MERGE}, {"MERGE-END:", 10, FLAGS_MANPAGE_SECTION_MERGE}};
  for (size_t i = 0; i < sizeof(markers) / sizeof(markers[0]); ++i) {
    if (strncmp(p, markers[i].prefix, markers[i].length) == 0) {
      return markers[i].type;
    }
  }
  return FLAGS_MANPAGE_SECTION_UNMARKED;
}

static bool section_header(const char *line, char **name) {
  const char *p = line;
  while (isspace((unsigned char)*p))
    ++p;
  if (strncmp(p, ".SH", 3) != 0)
    return false;
  p += 3;
  while (isspace((unsigned char)*p))
    ++p;
  const char *end = p + strlen(p);
  while (end > p && (end[-1] == '\n' || end[-1] == '\r'))
    --end;
  *name = copy_trimmed(p, end);
  return *name != NULL;
}

static bool append(char **buffer, size_t *length, size_t *capacity, const char *line, size_t line_length) {
  if (*length + line_length + 1 > *capacity) {
    size_t next = *capacity;
    while (*length + line_length + 1 > next)
      next *= 2;
    char *grown = realloc(*buffer, next);
    if (!grown)
      return false;
    *buffer = grown;
    *capacity = next;
  }
  memcpy(*buffer + *length, line, line_length);
  *length += line_length;
  (*buffer)[*length] = '\0';
  return true;
}

static long read_line(FILE *input, char **line, size_t *capacity) {
  if (!*line) {
    *capacity = 256;
    *line = malloc(*capacity);
    if (!*line) return -1;
  }
  size_t length = 0;
  int character;
  while ((character = fgetc(input)) != EOF) {
    if (length + 2 > *capacity) {
      *capacity *= 2;
      char *grown = realloc(*line, *capacity);
      if (!grown) return -1;
      *line = grown;
    }
    (*line)[length++] = (char)character;
    if (character == '\n') break;
  }
  if (length == 0 && character == EOF) return -1;
  (*line)[length] = '\0';
  return (long)length;
}

bool flags_manpage_parser_parse(FILE *input, flags_manpage_section_t **sections, size_t *count) {
  if (!input || !sections || !count)
    return false;
  *sections = NULL;
  *count = 0;
  size_t capacity = 8;
  flags_manpage_section_t *result = calloc(capacity, sizeof(*result));
  char *line = NULL;
  size_t line_capacity = 0;
  flags_manpage_section_t *current = NULL;
  char *body = NULL;
  size_t body_length = 0, body_capacity = 256, line_number = 0;
  flags_manpage_section_type_t current_type = FLAGS_MANPAGE_SECTION_UNMARKED;
  bool marked = false;
  if (!result || !(body = malloc(body_capacity)))
    goto fail;
  while (read_line(input, &line, &line_capacity) >= 0) {
    ++line_number;
    flags_manpage_section_type_t type = marker_type(line);
    if (type != FLAGS_MANPAGE_SECTION_UNMARKED) {
      marked = strstr(line, "-START:") != NULL;
      current_type = marked ? type : FLAGS_MANPAGE_SECTION_UNMARKED;
    }
    char *name = NULL;
    if (section_header(line, &name)) {
      if (current) {
        current->content = body;
        current->content_length = body_length;
        body = malloc(body_capacity);
        if (!body)
          goto fail;
        body_length = 0;
      }
      if (*count == capacity) {
        capacity *= 2;
        flags_manpage_section_t *grown = realloc(result, capacity * sizeof(*result));
        if (!grown) {
          free(name);
          goto fail;
        }
        result = grown;
      }
      current = &result[(*count)++];
      memset(current, 0, sizeof(*current));
      current->name = name;
      current->start_line = current->end_line = line_number;
      current->type = marked ? current_type : FLAGS_MANPAGE_SECTION_UNMARKED;
      current->has_markers = marked;
    }
    if (current) {
      if (!append(&body, &body_length, &body_capacity, line, strlen(line)))
        goto fail;
      current->end_line = line_number;
    }
  }
  if (current) {
    current->content = body;
    current->content_length = body_length;
    body = NULL;
  }
  free(body);
  free(line);
  *sections = result;
  return true;
fail:
  free(body);
  free(line);
  flags_manpage_parser_free(result, *count);
  *sections = NULL;
  *count = 0;
  return false;
}

bool flags_manpage_parser_parse_memory(const char *content, size_t length, flags_manpage_section_t **sections, size_t *count) {
  if (!content || !length)
    return false;
  FILE *input = tmpfile();
  if (!input || fwrite(content, 1, length, input) != length || fseek(input, 0, SEEK_SET) != 0) {
    if (input)
      fclose(input);
    return false;
  }
  bool ok = flags_manpage_parser_parse(input, sections, count);
  fclose(input);
  return ok;
}

void flags_manpage_parser_free(flags_manpage_section_t *sections, size_t count) {
  if (!sections)
    return;
  for (size_t i = 0; i < count; ++i) {
    free(sections[i].name);
    free(sections[i].content);
  }
  free(sections);
}

const flags_manpage_section_t *flags_manpage_parser_find(const flags_manpage_section_t *sections, size_t count, const char *name) {
  if (!sections || !name)
    return NULL;
  for (size_t i = 0; i < count; ++i)
    if (sections[i].name && strcmp(sections[i].name, name) == 0)
      return &sections[i];
  return NULL;
}
