Main API
========

The main API manages typed option schemas, application-owned storage, live
values, validation, parsing, environment variables, runtime updates, and JSON
patches. A schema is immutable; a context owns the mutable parsing metadata
and statistics for one bound storage instance.

Schemas and option values
-------------------------

.. c:autodoc:: libflags/flags.h libflags/flags_version.h libflags/builder.h

Semantic parsers and validation
-------------------------------

.. c:autodoc:: libflags/values.h libflags/parsers.h

Runtime values and live updates
------------------------------

.. c:autodoc:: libflags/runtime.h

Environment variables
---------------------

Environment names, precedence, and application are part of the main context
API. They use the same option descriptors and typed storage as command-line
updates.

JSON patches and snapshots
--------------------------

.. c:autodoc:: libflags/json.h

Nested commands
---------------

.. c:autodoc:: libflags/commands.h
