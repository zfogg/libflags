"""Sphinx configuration for libflags' C API documentation."""

from pathlib import Path

project = "libflags"
copyright = "2026, Zachary Fogg"
author = "Zachary Fogg"
release = "master"
extensions = ["myst_parser", "hawkmoth", "hawkmoth.ext.javadoc"]
source_suffix = {".md": "markdown", ".rst": "restructuredtext"}
master_doc = "index"
exclude_patterns = ["_build", "generated"]
hawkmoth_root = str(Path(__file__).resolve().parents[1] / "include")
hawkmoth_compiler = "clang"
hawkmoth_autoconf = []
hawkmoth_clang = ["-std=c23", "-I", str(Path(__file__).resolve().parents[1] / "include")]
hawkmoth_transform_default = "javadoc"
html_theme = "alabaster"
html_title = "libflags"
html_short_title = "libflags"
html_static_path = ["_static"]
html_css_files = ["theme.css"]
html_theme_options = {"description": "Typed command-line and configuration primitives for C.", "github_user": "zfogg", "github_repo": "libflags", "github_button": True, "show_related": False, "show_powered_by": False}
nitpicky = False


def _mark_c_code_blocks(app, lines, transform, options):
    """Give Hawkmoth's converted @code blocks C syntax highlighting."""
    if transform != "javadoc":
        return
    for index, line in enumerate(lines):
        if line == ".. code-block::":
            lines[index] = ".. code-block:: c"


def setup(app):
    app.connect("hawkmoth-process-docstring", _mark_c_code_blocks, priority=900)
