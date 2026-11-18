function(libflags_configure_target target)
  target_include_directories(${target}
    PUBLIC
      $<BUILD_INTERFACE:${LIBFLAGS_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  target_compile_features(${target} PUBLIC c_std_11)
  if(LIBFLAGS_LIBPLATFORM_SOURCE_DIR AND EXISTS "${LIBFLAGS_LIBPLATFORM_SOURCE_DIR}/include")
    target_include_directories(${target} PUBLIC
      $<BUILD_INTERFACE:${LIBFLAGS_LIBPLATFORM_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  endif()
  if(LIBFLAGS_TOMLC17_SOURCE_DIR AND EXISTS "${LIBFLAGS_TOMLC17_SOURCE_DIR}/src")
    target_include_directories(${target} PUBLIC
      $<BUILD_INTERFACE:${LIBFLAGS_TOMLC17_SOURCE_DIR}/src>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  endif()
  # These implementation dependencies must remain transitive for static
  # consumers, but their usage requirements must not become compile includes
  # for libflags itself (especially host /usr/include paths in musl builds).
  target_link_libraries(${target} PUBLIC
    "$<LINK_ONLY:libplatform::libplatform>"
    "$<LINK_ONLY:libflags-tomlc17>")
  if(LIBFLAGS_UTF8PROC_TARGET AND TARGET "${LIBFLAGS_UTF8PROC_TARGET}")
    target_link_libraries(${target} PUBLIC "${LIBFLAGS_UTF8PROC_TARGET}")
  endif()
  if(WIN32)
    target_link_libraries(${target} PUBLIC ws2_32)
  endif()
  if(LIBFLAGS_PCRE2_INCLUDE_DIRS)
    target_include_directories(${target} PRIVATE ${LIBFLAGS_PCRE2_INCLUDE_DIRS})
    target_compile_definitions(${target} PUBLIC LIBFLAGS_HAS_PCRE2=1 PCRE2_STATIC=1)
  endif()
  if(LIBFLAGS_PCRE2_LIBRARY_DIRS)
    target_link_directories(${target} PRIVATE ${LIBFLAGS_PCRE2_LIBRARY_DIRS})
  endif()
  if(LIBFLAGS_PCRE2_TARGET AND TARGET "${LIBFLAGS_PCRE2_TARGET}")
    target_link_libraries(${target} PUBLIC "${LIBFLAGS_PCRE2_TARGET}")
  elseif(LIBFLAGS_PCRE2_LIBRARIES)
    target_link_libraries(${target} PUBLIC ${LIBFLAGS_PCRE2_LIBRARIES})
  endif()
  if(LIBFLAGS_YYJSON_INCLUDE_DIRS)
    target_include_directories(${target} PRIVATE ${LIBFLAGS_YYJSON_INCLUDE_DIRS})
    target_compile_definitions(${target} PUBLIC LIBFLAGS_HAS_YYJSON=1)
  endif()
  if(LIBFLAGS_YYJSON_TARGET AND TARGET "${LIBFLAGS_YYJSON_TARGET}")
    target_link_libraries(${target} PUBLIC "${LIBFLAGS_YYJSON_TARGET}")
  elseif(LIBFLAGS_YYJSON_LIBRARIES)
    target_link_libraries(${target} PUBLIC ${LIBFLAGS_YYJSON_LIBRARIES})
  endif()
endfunction()
