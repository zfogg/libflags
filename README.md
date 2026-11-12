# libflags

Reusable, typed C command-line schemas and parsing. `--` is always the end-of-options marker; a single `-` remains a value. Applications own storage and stream opening.

The library owns generic descriptors, aliases, precedence, environment derivation,
runtime metadata, TOML configuration import/export, shell completion output, and
generic groff option sections. Applications retain concrete storage, option
registries, command descriptions, mode-specific validation, configuration prose,
and branded manpage sections.

## Embedding PCRE2

Regex validation and URL validation use PCRE2 when the embedding project supplies
the existing PCRE2 build to CMake:

```text
-DLIBFLAGS_PCRE2_INCLUDE_DIRS=/path/to/pcre2/include
-DLIBFLAGS_PCRE2_LIBRARIES=pcre2-8
```

The embedding project owns the PCRE2 source, target, and runtime deployment.
ascii-chat passes its already-configured PCRE2 target to libflags, so embedding
libflags does not create a second PCRE2 build. When these variables are omitted,
the library still builds without PCRE2; `flags_validate_regex()` reports failure
until PCRE2 is supplied.

Reusable network and URL helpers are available under `include/libflags/util/`.
