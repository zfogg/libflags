# libflags documentation

The public API is documented in the headers and published from `main` by the
GitHub Pages workflow. The parser contract is deliberately small: `--` ends
option parsing, while `-` is a normal value that may be classified as stdin or
stdout by option metadata.
