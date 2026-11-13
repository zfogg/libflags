Core API
========

These headers define schemas, typed values, parsing, validation, runtime
updates, configuration, JSON adapters, completions, and generic rendering.

.. c:autodoc:: libflags/flags.h libflags/flags_types.h libflags/flags_schema.h libflags/flags_parser.h libflags/flags_completion.h libflags/flags_manpage.h libflags/flags_version.h libflags/common.h libflags/enums.h libflags/options.h libflags/schema.h libflags/values.h libflags/parsers.h libflags/validation.h libflags/runtime.h libflags/commands.h libflags/config.h libflags/json.h libflags/presets.h libflags/help.h libflags/layout.h libflags/levenshtein.h libflags/log.h

Builder and parsing APIs
------------------------

.. c:autodoc:: libflags/builder.h libflags/builder/api.h libflags/builder/builder.h libflags/builder/metadata.h libflags/parsing/actions.h libflags/parsing/parsers.h libflags/parsing/values.h

Configuration and completion APIs
---------------------------------

.. c:autodoc:: libflags/config/environment.h libflags/config/presets.h libflags/config/schema.h libflags/completions/bash.h libflags/completions/completions.h libflags/completions/fish.h libflags/completions/powershell.h libflags/completions/zsh.h

Manpage APIs
------------

.. c:autodoc:: libflags/manpage/formatter.h libflags/manpage/manpage.h libflags/manpage/merger.h libflags/manpage/parser.h libflags/manpage/resources.h

Utilities
---------

.. c:autodoc:: libflags/util/ip.h libflags/util/url.h
