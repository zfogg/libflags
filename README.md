# libflags

Reusable, typed C command-line schemas and parsing. `--` is always the end-of-options marker; a single `-` remains a value. Applications own storage and stream opening.

The library owns generic descriptors, aliases, precedence, environment derivation,
runtime metadata, TOML configuration import/export, shell completion output, and
generic groff option sections. Applications retain concrete storage, option
registries, command descriptions, mode-specific validation, configuration prose,
and branded manpage sections.
