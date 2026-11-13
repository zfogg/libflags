add_custom_target(libflags-docs
  COMMAND ${CMAKE_COMMAND} -E env
    uv run --with-requirements ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    sphinx-build --keep-going -b html ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/html
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
  COMMENT "Building libflags Sphinx documentation")
