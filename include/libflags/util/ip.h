/** @file
 * @brief Portable IP address validation and host/port parsing helpers.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

/** Returns nonzero when @p text is a valid IPv4 address. */
int flags_is_valid_ipv4(const char *text);
/** Returns nonzero when @p text is a valid IPv6 address, with optional brackets. */
int flags_is_valid_ipv6(const char *text);
/** Extracts an address and port from `host:port` or `[ipv6]:port`. */
int flags_parse_ip_with_port(const char *text, char *address, size_t address_size, uint16_t *port);
/** Extracts an address and optional port, using @p default_port when absent. */
int flags_parse_address_with_optional_port(const char *text, char *address, size_t address_size, uint16_t *port, uint16_t default_port);
