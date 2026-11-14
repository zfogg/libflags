Core API
========

These headers define schemas, typed values, parsing, validation, runtime
updates, configuration, JSON adapters, completions, and generic rendering.

.. c:autodoc:: libflags/flags.h libflags/flags_version.h libflags/values.h libflags/parsers.h libflags/validation.h libflags/runtime.h libflags/commands.h

Configuration and serialization
-------------------------------

The configuration and JSON adapters use the same typed schema and precedence
rules as command-line parsing.

.. c:autodoc:: libflags/config.h libflags/json.h

Builder and parsing APIs
------------------------

.. c:autodoc:: libflags/builder.h

Configuration and completion APIs
---------------------------------

.. c:autodoc:: libflags/completions/completions.h

Manpage APIs
------------

.. c:autodoc:: libflags/manpage/formatter.h libflags/manpage/manpage.h

Utilities
---------

.. c:autodoc:: libflags/util/ip.h libflags/util/url.h
