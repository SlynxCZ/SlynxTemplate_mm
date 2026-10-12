# Settings every platform shares -- CMake twin of makefiles/ambuild/shared.ambuild.
# The per-platform ones are in linux.cmake / windows.cmake.

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif ()

if (WIN32 AND NOT MSVC)
    message(FATAL "MSVC restricted.")
endif ()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING
        "Only do Release and Debug"
        FORCE
)

set(CMAKE_CXX_STANDARD 20)

if (LINUX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif ()

set(CMAKE_STATIC_LIBRARY_PREFIX "")

# s2sdk's include paths, defines and libraries come from s2sdk-manifests, as
# s2sdk::s2sdk (linked in CMakeBuilder.cmake) -- SdkHelpers.configureCxx() on the
# AMBuild side.

add_definitions(-DMETA_IS_SOURCE2)

if (DEFINED ENV{GITHUB_SHA_SHORT})
    add_definitions(-DGITHUB_SHA="$ENV{GITHUB_SHA_SHORT}")
else ()
    add_definitions(-DGITHUB_SHA="Local")
endif ()

if (DEFINED ENV{SEMVER})
    add_definitions(-DSEMVER="$ENV{SEMVER}")
else ()
    add_definitions(-DSEMVER="Local")
endif ()

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/protobuf
    ${CMAKE_SOURCE_DIR}/src/sdk
    ${METAMOD_DIR}/core
    ${METAMOD_DIR}/third_party/khook/include
    # NOT ${METAMOD_DIR}/core/sourcehook: this plugin brings its own private
    # SourceHook (vendor/sourcehook submodule). Its include dirs come from
    # the add_subdirectory(vendor/sourcehook) target in CMakeLists.txt
    # (PUBLIC, so nothing to list here), including the shim that keeps
    # metamod's own ISmmPlugin.h `#include "sourcehook.h"` resolving -- to
    # our copy. See src/plugin.h's include of
    # sourcehook/sourcehook_metamod_override.h for the caveat this carries.
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_SOURCE_DIR}/vendor/dynlibutils/include
    ${CMAKE_SOURCE_DIR}/vendor/dynlibutils/include/dynlibutils
)
