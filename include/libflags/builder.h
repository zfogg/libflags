/** @file
 * @brief Mutable construction of immutable-looking libflags schemas.
 */
#pragma once
#include <libflags/flags.h>

typedef struct flags_schema_builder flags_schema_builder_t;
/**
 * @brief Creates a schema builder with independent descriptor storage.
 * @return New builder, or NULL on allocation failure.
 * @note Descriptors are copied, but pointed-to strings, enum tables,
 * callbacks, and callback data remain owned by the caller.
 */
flags_schema_builder_t *flags_schema_builder_create(void);
/**
 * @brief Releases a builder and its copied descriptor arrays.
 * @param builder Builder to release.
 */
void flags_schema_builder_destroy(flags_schema_builder_t *builder);
/**
 * @brief Appends an option descriptor by value.
 * @param builder Builder to update.
 * @param option Descriptor to copy.
 * @return Whether the descriptor was appended.
 * @note String members, enum tables, callbacks, and callback data remain
 * application-owned and must outlive the builder's schema view.
 */
bool flags_schema_builder_add_option(flags_schema_builder_t *builder, const flags_option_spec_t *option);
/**
 * @brief Appends a positional descriptor by value.
 * @param builder Builder to update.
 * @param positional Descriptor to copy.
 * @return Whether the descriptor was appended.
 */
bool flags_schema_builder_add_positional(flags_schema_builder_t *builder, const flags_positional_spec_t *positional);
/**
 * @brief Selects the ordering policy used by the resulting schema.
 * @param builder Builder to update.
 * @param order Ordering policy.
 * @return Whether the policy was accepted.
 */
bool flags_schema_builder_set_argument_order(flags_schema_builder_t *builder, flags_argument_order_t order);
/**
 * @brief Returns the builder-owned schema view.
 * @param builder Builder to inspect.
 * @return Borrowed schema view, or NULL for invalid input.
 * @note The pointer remains valid until the next builder mutation or
 * destruction. Do not destroy the builder while a context uses this schema.
 */
const flags_schema_t *flags_schema_builder_schema(const flags_schema_builder_t *builder);
