# libflags documentation

libflags is a reusable, typed command-line and configuration engine. It owns
schema metadata, parsing, precedence, validation, runtime updates, JSON
adapters, completions, and generic man-page generation; applications own their
commands, examples, branding, and application-specific validation.

The public API is documented in the headers and published from `main` by the
GitHub Pages workflow. The parser contract is deliberately small: `--` ends
option parsing, while `-` is a normal value that may be classified as stdin or
stdout by option metadata.

`flags_config_load_toml()` and `flags_config_write_toml()` operate on dotted
option names at configuration precedence. `flags_completion_write()` generates
Bash, Zsh, or Fish syntax from schema metadata, and `flags_manpage_write()`
generates the generic option section. Host applications retain their commands,
examples, environment prose, and mode-specific validation.

## Design topics

- [Architecture](topics/architecture.dox)
- [Schema and typed storage](topics/schema.dox)
- [Parsing and precedence](topics/parsing.dox)
- [Runtime updates](topics/runtime.dox)
- [Commands and positionals](topics/commands.dox)
- [JSON integration](topics/json.dox)
- [Generated artifacts](topics/generators.dox)
