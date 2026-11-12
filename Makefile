SOURCES := $(shell find . -type f \( -name '*.c' -o -name '*.h' \))
.PHONY: format format-check
format:
	clang-format -i $(SOURCES)
format-check:
	clang-format --dry-run --Werror $(SOURCES)
