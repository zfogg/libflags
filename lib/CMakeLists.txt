set(LIBFLAGS_SOURCES
  options.c
  json.c
  log.c
  commands.c
  parsing/parsers.c
  builder/builder.c
  config/config.c
  completions/completions.c
  manpage/manpage.c
  manpage/formatter.c)

add_library(flags ${LIBFLAGS_SOURCES})
add_library(libflags::flags ALIAS flags)
libflags_configure_target(flags)
