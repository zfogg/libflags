# Architecture

libflags provides independent contexts for schema, typed storage, precedence,
diagnostics, and runtime metadata. Host applications retain commands,
branding, side effects, and application-specific validation.

The immutable schema can be shared by multiple contexts; mutable values,
owned strings, revisions, callbacks, and statistics belong to each context.
