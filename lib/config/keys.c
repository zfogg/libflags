#include <libflags/config/keys.h>

#include <ctype.h>
#include <stdint.h>
#include <string.h>

bool flags_config_make_toml_key(const char *group, const char *field_name, char *buffer, size_t buffer_size) {
  if (!group || !field_name || !buffer || buffer_size == 0)
    return false;
  size_t group_length = strlen(group);
  size_t field_length = strlen(field_name);
  if (group_length > SIZE_MAX - field_length - 2 || group_length + field_length + 2 > buffer_size)
    return false;
  size_t position = 0;
  for (size_t i = 0; i < group_length; ++i)
    buffer[position++] = (char)tolower((unsigned char)group[i]);
  buffer[position++] = '.';
  for (size_t i = 0; i < field_length; ++i) {
    char value = field_name[i] == '-' ? '_' : field_name[i];
    buffer[position++] = (char)tolower((unsigned char)value);
  }
  buffer[position] = '\0';
  return true;
}

bool flags_config_make_cli_flag(const char *name, char *buffer, size_t buffer_size) {
  if (!name || !buffer || buffer_size < 3)
    return false;
  size_t length = strlen(name);
  if (length > buffer_size - 3)
    return false;
  buffer[0] = '-';
  buffer[1] = '-';
  memcpy(buffer + 2, name, length + 1);
  return true;
}
