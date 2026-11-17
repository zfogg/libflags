set(LIBFLAGS_SOURCES
  options.c
  explicit.c
  format.c
  help.c
  json.c
  log.c
  commands.c
  levenshtein.c
  parsing/parsers.c
  builder/builder.c
  config/config.c
    config/keys.c
    storage.c
  completions/completions.c
  manpage/manpage.c
  manpage/formatter.c
  manpage/parser.c
)

add_library(flags ${LIBFLAGS_SOURCES})
add_library(libflags::flags ALIAS flags)
set_target_properties(flags PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  WINDOWS_EXPORT_ALL_SYMBOLS ON)
libflags_configure_target(flags)
