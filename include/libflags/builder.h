/** @file
 * @brief Mutable construction of immutable-looking libflags schemas.
 */
#pragma once
#include <libflags/flags.h>

typedef struct flags_schema_builder flags_schema_builder_t;
/**
 * Creates a schema builder with independent descriptor storage.
 *
 * Descriptors are copied into the builder, but pointed-to strings, enum
 * tables, callbacks, and callback data remain owned by the caller.
 */
flags_schema_builder_t *flags_schema_builder_create(void);
/** Releases a builder and its copied descriptor arrays. */
void flags_schema_builder_destroy(flags_schema_builder_t *builder);
/**
 * Appends an option descriptor by value.
 *
 * String members, enum tables, callbacks, and callback data remain
 * application-owned and must outlive the builder's schema view.
 */
bool flags_schema_builder_add_option(flags_schema_builder_t *builder, const flags_option_spec_t *option);
/** Appends a positional descriptor by value. */
bool flags_schema_builder_add_positional(flags_schema_builder_t *builder, const flags_positional_spec_t *positional);
/** Selects the ordering policy used by the resulting schema. */
bool flags_schema_builder_set_argument_order(flags_schema_builder_t *builder, flags_argument_order_t order);
/**
 * Returns the builder-owned schema view.
 *
 * The pointer remains valid until the next builder mutation or destruction.
 * Do not destroy the builder while a context is using this schema.
 */
const flags_schema_t *flags_schema_builder_schema(const flags_schema_builder_t *builder);
