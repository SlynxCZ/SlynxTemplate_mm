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

if(DEFINED ENV{CSGO_PROTO})
    set(CSGO_PROTO_DIR $ENV{CSGO_PROTO})
else()
    message(FATAL_ERROR "Environment variable CSGO_PROTO is not set!")
endif()

include(makefiles/shared.cmake)
include(makefiles/protobuf.cmake)

# This plugin's own private SourceHook, not metamod's shared one -- see
# makefiles/shared.cmake, src/plugin.h and README.md. Its own test suite
# stays out of this build.
set(SOURCEHOOK_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/sourcehook)
add_subdirectory(vendor/dynlibutils)

file(GLOB_RECURSE SOURCE_FILES
    src/*.cpp
    src/*.h
    src/*.hpp
)

list(APPEND SOURCE_FILES
    ${SOURCESDK_DIR}/public/tier0/memoverride.cpp
    ${SOURCESDK_DIR}/tier1/bitbuf.cpp
    ${SOURCESDK_DIR}/tier1/convar.cpp
    ${SOURCESDK_DIR}/tier1/keyvalues3.cpp
    ${SOURCESDK_DIR}/entity2/entityidentity.cpp
    ${SOURCESDK_DIR}/entity2/entitykeyvalues.cpp
    ${SOURCESDK_DIR}/entity2/entitysystem.cpp
)

foreach(f ${SOURCE_FILES})
    message(STATUS "Zdrojový soubor: ${f}")
endforeach()

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

find_package(Git QUIET)
if (GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_SHA
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
else()
    set(GIT_SHA "nogit")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    find_program(STRIP_EXECUTABLE strip)
    if (STRIP_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${STRIP_EXECUTABLE} --strip-unneeded $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Stripping symbols from $<TARGET_FILE:${PROJECT_NAME}>"
        )
    endif()
endif()

if (LINUX)
    include(${CMAKE_SOURCE_DIR}/makefiles/linux.base.cmake)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/slynx_template/bin/linuxsteamrt64"
    )
elseif (WIN32)
    include(${CMAKE_SOURCE_DIR}/makefiles/windows.base.cmake)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/slynx_template/bin/win64"
    )
endif()

target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/configs ${CMAKE_BINARY_DIR}
)
