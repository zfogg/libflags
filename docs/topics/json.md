# JSON and TOML integration

JSON and TOML are adapters over the typed schema, not the storage model.
Imports validate declared types, aliases, regex constraints, and precedence
before committing values. Applications remain responsible for branded prose
and policy-specific diagnostics.
