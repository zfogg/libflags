# Parsing and precedence

The parser handles long and short options, aliases, nested commands, positionals,
the `--` terminator, and the lone `-` as data. Values are resolved through
the same schema lookup regardless of whether they came from CLI, environment,
configuration, or runtime code.

Source precedence is checked again while committing under the context lock.
