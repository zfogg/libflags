SOURCES := $(shell git ls-files '*.c' '*.h')
.PHONY: format format-check
format:
	clang-format -i $(SOURCES)
format-check:
	clang-format --dry-run --Werror $(SOURCES)
