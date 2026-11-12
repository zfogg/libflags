#include <libflags/util/ip.h>

#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

int flags_is_valid_ipv4(const char *text) {
  struct in_addr address;
  return text && inet_pton(AF_INET, text, &address) == 1;
}

int flags_is_valid_ipv6(const char *text) {
  char value[64];
  size_t length;
  struct in6_addr address;
  if (!text)
    return 0;
  length = strlen(text);
  if (length >= 2 && text[0] == '[' && text[length - 1] == ']') {
    length -= 2;
    if (length >= sizeof(value))
      return 0;
    memcpy(value, text + 1, length);
    value[length] = '\0';
    text = value;
  }
  return inet_pton(AF_INET6, text, &address) == 1;
}

static int copy_address(const char *source, size_t length, char *destination, size_t capacity) {
  if (!source || !destination || capacity == 0 || length >= capacity)
    return -1;
  memcpy(destination, source, length);
  destination[length] = '\0';
  return 0;
}

static int parse_port(const char *text, uint16_t *port) {
  unsigned long value = 0;
  if (!text || !*text || !port)
    return -1;
  for (; *text; ++text) {
    if (*text < '0' || *text > '9' || value > 6553)
      return -1;
    value = value * 10 + (unsigned long)(*text - '0');
  }
  if (value > 65535)
    return -1;
  *port = (uint16_t)value;
  return 0;
}

int flags_parse_ip_with_port(const char *text, char *address, size_t address_size, uint16_t *port) {
  const char *separator;
  if (!text || !address || !port)
    return -1;
  if (text[0] == '[') {
    const char *closing = strchr(text, ']');
    if (!closing || closing[1] != ':')
      return -1;
    if (copy_address(text + 1, (size_t)(closing - text - 1), address, address_size) != 0)
      return -1;
    return parse_port(closing + 2, port);
  }
  separator = strrchr(text, ':');
  if (!separator || strchr(text, ':') != separator)
    return -1;
  if (copy_address(text, (size_t)(separator - text), address, address_size) != 0)
    return -1;
  return parse_port(separator + 1, port);
}

int flags_parse_address_with_optional_port(const char *text, char *address, size_t address_size, uint16_t *port,
                                          uint16_t default_port) {
  size_t length;
  if (!text || !address || !port)
    return -1;
  length = strlen(text);
  if (length >= 2 && text[0] == '[' && text[length - 1] == ']') {
    if (copy_address(text + 1, length - 2, address, address_size) != 0)
      return -1;
    *port = default_port;
    return 0;
  }
  if (strchr(text, ':') && flags_parse_ip_with_port(text, address, address_size, port) == 0)
    return 0;
  if (copy_address(text, length, address, address_size) != 0)
    return -1;
  *port = default_port;
  return 0;
}
