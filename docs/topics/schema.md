# Schema and typed storage

An option descriptor declares its type, storage binding, aliases, defaults,
environment mapping, validation, and callbacks. The descriptor API is the
stable generic boundary; applications may layer convenience macros over it.

Typed storage is checked before assignment so configuration and JSON imports
cannot write a value through an incompatible descriptor.
