SOURCES := $(shell git ls-files '*.c' '*.h')
.PHONY: docs format format-check

docs:
	uv run --with-requirements docs/requirements.txt sphinx-build --keep-going -b html docs docs/_build/html

format:
	clang-format -i $(SOURCES)
format-check:
	clang-format --dry-run --Werror $(SOURCES)
