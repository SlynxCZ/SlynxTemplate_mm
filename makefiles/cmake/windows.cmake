# Windows (MSVC) settings -- CMake twin of makefiles/ambuild/windows.ambuild.

# COMPILER_MSVC*, WIN32, WIN64, _ITERATOR_DEBUG_LEVEL=0 and the SDK's libraries
# come from s2sdk-manifests' windows.cmake, through s2sdk::s2sdk.
add_definitions(
    -D_WIN32 -D_WINDOWS -D_ALLOW_KEYWORD_MACROS -D__STDC_LIMIT_MACROS
    -D_CRT_SECURE_NO_WARNINGS=1 -D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NONSTDC_NO_DEPRECATE=1
)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4819 /wd4828 /wd5033 /permissive- /utf-8 /wd4005 /MP")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libcmt")

set(LINK_LIBRARIES
    s2sdk::s2sdk
    dynlibutils
    sourcehook
)