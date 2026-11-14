# libflags

[View the source on GitHub](https://github.com/zfogg/libflags)

libflags is a reusable, typed command-line and configuration engine for C.
It owns schema metadata, parsing, precedence, validation, runtime updates,
JSON and TOML adapters, completions, and generic man-page generation.
Applications retain their commands, examples, branding, and policy.

The public headers are the reference. The API pages below are generated from
those headers with Hawkmoth, so declarations and documentation stay together.

```{toctree}
:maxdepth: 2

api
topics/architecture
topics/schema
topics/parsing
topics/runtime
topics/commands
topics/json
topics/generators
```

## Design boundary

An application owns its concrete storage and option registry. A libflags
schema describes how that storage is typed, parsed, validated, serialized,
and exposed to generic tools. Multiple contexts can share an immutable schema
without sharing mutable values or runtime statistics.

## Header map

- `flags.h`, `flags_types.h`, and `flags_schema.h` — core public types and descriptors.
- `flags_parser.h`, `parsing/`, and `parsers.h` — command-line and text parsing.
- `runtime.h` — synchronized reads, writes, revisions, and atomic batches.
- `config.h`, `json.h`, and `config/` — configuration adapters.
- `completions/` and `manpage/` — generic generated shell and documentation output,
  including reusable usage, options, positional, environment, and examples sections.
- `util/ip.h` and `util/url.h` — reusable network-string helpers.
