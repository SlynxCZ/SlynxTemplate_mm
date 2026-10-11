# The install layout -- CMake twin of makefiles/ambuild/package.ambuild: the
# binary in addons/slynx_template/bin/<platform>/, and configs/ copied over the
# build root, carrying the metamod .vdf metamod.cmake wrote into it.
if (WIN32)
    set(PLUGIN_BIN_PLATFORM "win64")
else ()
    set(PLUGIN_BIN_PLATFORM "linuxsteamrt64")
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/slynx_template/bin/${PLUGIN_BIN_PLATFORM}"
)

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/configs ${CMAKE_BINARY_DIR}
)
