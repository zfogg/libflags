# Commands and positionals

Commands are schema nodes with arbitrary nesting depth. A selected command
inherits options from its parents, allowing global flags and command-specific
registries without making libflags aware of application modes.
