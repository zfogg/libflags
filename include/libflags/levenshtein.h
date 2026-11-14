/** @file
 * @brief Suggestion-generation integration boundary.
 *
 * This facade reserves the public home for spelling suggestions used by help
 * and diagnostics. Suggestion generation must not alter schema or bound
 * option storage.
 */
#pragma once
#include <libflags/flags.h>
