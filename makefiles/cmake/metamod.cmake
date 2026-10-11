# The plugin's metamod .vdf -- CMake twin of makefiles/ambuild/metamod.ambuild.
# Both fill in the same makefiles/metamod/<name>.vdf.in; this writes it to
# configs/addons/metamod/, which package.cmake copies into the build tree.

if (WIN32)
	set(PROJECT_VDF_PLATFORM "win64")
else()
	set(PROJECT_VDF_PLATFORM "linuxsteamrt64")
endif()

configure_file(
		${CMAKE_CURRENT_LIST_DIR}/../metamod/slynx_template.vdf.in
	${PROJECT_SOURCE_DIR}/configs/addons/metamod/slynx_template.vdf
)
