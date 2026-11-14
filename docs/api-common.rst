Core API
========

These headers define schemas, typed values, parsing, validation, runtime
updates, configuration, JSON adapters, completions, and generic rendering.

Core types and value parsing
----------------------------

Schemas, typed values, semantic parsers, and validation primitives.

.. c:autodoc:: libflags/flags.h libflags/flags_version.h libflags/values.h libflags/parsers.h libflags/validation.h

Runtime and command APIs
------------------------

Context lifecycle, runtime updates, diagnostics, and nested command
resolution.

.. c:autodoc:: libflags/runtime.h libflags/commands.h

Configuration and serialization
-------------------------------

The configuration and JSON adapters use the same typed schema and precedence
rules as command-line parsing.

.. c:autodoc:: libflags/config.h libflags/json.h

Schema builder
--------------

Mutable construction of the immutable schema view consumed by parsing and
runtime contexts.

.. c:autodoc:: libflags/builder.h

Shell completion
----------------

Schema-driven completion generation for supported shells.

.. c:autodoc:: libflags/completions/completions.h

Manpage generation
------------------

Groff formatting and schema-derived option documentation.

.. c:autodoc:: libflags/manpage/formatter.h libflags/manpage/manpage.h

Network utilities
-----------------

Portable IP address and URL validation helpers.

.. c:autodoc:: libflags/util/ip.h libflags/util/url.h
