# libflags documentation

The public API is documented in the headers and published from `main` by the
GitHub Pages workflow. The parser contract is deliberately small: `--` ends
option parsing, while `-` is a normal value that may be classified as stdin or
stdout by option metadata.

`flags_config_load_toml()` and `flags_config_write_toml()` operate on dotted
option names at configuration precedence. `flags_completion_write()` generates
Bash, Zsh, or Fish syntax from schema metadata, and `flags_manpage_write()`
generates the generic option section. Host applications retain their commands,
examples, environment prose, and mode-specific validation.
