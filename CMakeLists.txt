cmake_minimum_required(VERSION 3.18)
project(slynx_template C CXX ASM)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# s2sdk (alliedmodders/s2sdk, formerly hl2sdk's cs2 branch): the first of
# these variables that is set wins.
foreach(var S2SDK_CS2 S2SDKCS2 S2SDK HL2SDK_CS2 HL2SDKCS2)
    if(DEFINED ENV{${var}} AND NOT "$ENV{${var}}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{${var}}" SOURCESDK_DIR)
        break()
    endif()
endforeach()
if(NOT SOURCESDK_DIR)
    message(FATAL_ERROR "s2sdk not found: set S2SDK (or HL2SDKCS2) to your checkout!")
endif()

if(DEFINED ENV{MMSOURCE_DEV})
    set(METAMOD_DIR $ENV{MMSOURCE_DEV})
else()
    message(FATAL_ERROR "Environment variable MMSOURCE_DEV is not set!")
endif()

include(makefiles/cmake/shared.cmake)
# s2sdk-manifests: s2sdk::s2sdk (s2sdk's include paths, defines and libraries)
# and s2sdk_protobufs(). S2SDK_MANIFESTS_DIR as a cache or environment variable,
# else fetched.
if (NOT S2SDK_MANIFESTS_DIR AND DEFINED ENV{S2SDK_MANIFESTS_DIR})
    file(TO_CMAKE_PATH "$ENV{S2SDK_MANIFESTS_DIR}" S2SDK_MANIFESTS_DIR)
endif ()
if (NOT S2SDK_MANIFESTS_DIR OR NOT EXISTS "${S2SDK_MANIFESTS_DIR}/s2sdk-manifests.cmake")
    include(FetchContent)
    FetchContent_Declare(s2sdk_manifests
            GIT_REPOSITORY https://github.com/Source2Mod/s2sdk-manifests.git
            GIT_TAG master
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(s2sdk_manifests)
    set(S2SDK_MANIFESTS_DIR "${s2sdk_manifests_SOURCE_DIR}")
endif ()
include("${S2SDK_MANIFESTS_DIR}/s2sdk-manifests.cmake")
s2sdk_manifest(cs2 "${SOURCESDK_DIR}")

# This plugin's own private SourceHook, not metamod's shared one -- see
# makefiles/cmake/shared.cmake, src/plugin.h and README.md. Its own test suite
# stays out of this build.
set(SOURCEHOOK_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/sourcehook)
add_subdirectory(vendor/dynlibutils)

if (LINUX)
    include(${CMAKE_SOURCE_DIR}/makefiles/cmake/linux.cmake)
elseif (WIN32)
    include(${CMAKE_SOURCE_DIR}/makefiles/cmake/windows.cmake)
endif()

# The metamod .vdf from makefiles/metamod/ (metamod.ambuild on the AMBuild side).
include(${CMAKE_SOURCE_DIR}/makefiles/cmake/metamod.cmake)

# The plugin binary (makefiles/ambuild/AMBuilder on the AMBuild side).
include(${CMAKE_SOURCE_DIR}/makefiles/cmake/CMakeBuilder.cmake)

# Install layout (makefiles/ambuild/package.ambuild on the AMBuild side).
include(${CMAKE_SOURCE_DIR}/makefiles/cmake/package.cmake)
