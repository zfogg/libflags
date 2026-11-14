Core API
========

These headers define schemas, typed values, parsing, validation, runtime
updates, configuration, JSON adapters, completions, and generic rendering.

Core types
----------

Typed schemas, contexts, and option descriptors.

.. c:autodoc:: libflags/flags.h libflags/flags_version.h

Semantic value parsing
----------------------

Common typed-value parsers and validation primitives.

.. c:autodoc:: libflags/values.h libflags/parsers.h libflags/validation.h

Runtime and command APIs
------------------------

Context lifecycle, runtime updates, diagnostics, and nested command
resolution.

.. c:autodoc:: libflags/runtime.h

Nested command resolution
~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:autodoc:: libflags/commands.h

Configuration and serialization
-------------------------------

The configuration and JSON adapters use the same typed schema and precedence
rules as command-line parsing.

TOML configuration
~~~~~~~~~~~~~~~~~~

.. c:autodoc:: libflags/config.h

JSON integration
~~~~~~~~~~~~~~~~

.. c:autodoc:: libflags/json.h

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

Groff formatting primitives
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:autodoc:: libflags/manpage/formatter.h

Schema-derived option sections
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:autodoc:: libflags/manpage/manpage.h

Network utilities
-----------------

IPv4, IPv6, and host/port parsing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:autodoc:: libflags/util/ip.h

URL validation
~~~~~~~~~~~~~~

.. c:autodoc:: libflags/util/url.h
