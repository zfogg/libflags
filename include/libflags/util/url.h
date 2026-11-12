/** @file
 * @brief Generic URL validation helpers.
 */
#pragma once

#include <stdbool.h>

/** Returns true for a syntactically valid HTTP(S), WebSocket, or TCP URL. */
bool flags_url_is_valid(const char *url);
/** Returns true when @p scheme is a WebSocket scheme. */
bool flags_url_is_websocket_scheme(const char *scheme);
/** Returns true when @p url uses ws or wss. */
bool flags_url_is_websocket(const char *url);
/** Returns true when @p scheme is a TCP scheme. */
bool flags_url_is_tcp_scheme(const char *scheme);
/** Returns true when @p url uses tcp. */
bool flags_url_is_tcp(const char *url);
