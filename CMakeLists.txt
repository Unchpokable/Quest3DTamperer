cmake_minimum_required(VERSION 3.21)

project(Quest3DTamperer LANGUAGES CXX)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR
        "Quest3DTamperer targets x86 only: Audiosurf is a 32-bit process, so a "
        "64-bit build of this DLL could never be injected into it. Configure "
        "with a 32-bit toolset (e.g. the x86-* CMake presets, or run from an "
        "\"x86 Native Tools Command Prompt for VS\").")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Matches the original vcxproj's CharacterSet=Unicode, applied build-wide so
# imgui (which branches on _UNICODE) agrees with our own code.
add_compile_definitions(UNICODE _UNICODE)

include(cmake/Detours.cmake)
include(cmake/DirectXSDK.cmake)
include(cmake/Quest3DSDK.cmake)

add_subdirectory(Quest3DTamperer/third_party/imgui)
add_subdirectory(Quest3DTamperer/third_party/UGraphviz)

add_library(Quest3DTamperer SHARED
    Quest3DTamperer/src/main.cpp
    Quest3DTamperer/src/hooks/channel_hook.cpp
    Quest3DTamperer/src/hooks/d3d9_hooks.cpp
    Quest3DTamperer/src/hooks/detour_transaction.cpp
    Quest3DTamperer/src/quest3d/api.cpp
    Quest3DTamperer/src/quest3d/state.cpp
    Quest3DTamperer/src/ui/tamperer_window.cpp
    Quest3DTamperer/src/graph/channel_graph_dump.cpp
    Quest3DTamperer/src/util/text_convert.cpp
    Quest3DTamperer/src/util/clipboard.cpp
)

target_include_directories(Quest3DTamperer PRIVATE Quest3DTamperer/src)

target_link_libraries(Quest3DTamperer PRIVATE
    detours
    dxsdk
    imgui
    UGraphviz
    quest3d_sdk
)

target_precompile_headers(Quest3DTamperer PRIVATE Quest3DTamperer/src/pch.h)

# Matches the stack size the original vcxproj configured.
target_link_options(Quest3DTamperer PRIVATE /STACK:4194304)

set_target_properties(Quest3DTamperer PROPERTIES
    OUTPUT_NAME "Quest3DTamperer"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# There's one compile_commands.json per preset (build/x86-debug, build/x86-release,
# ...), but tooling (clangd, clang-tidy) wants a single stable path. Keep
# build/compile_commands.json pointed at whichever preset was built most
# recently by copying it there on every build.
add_custom_target(copy_compile_commands ALL
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/build/compile_commands.json"
    BYPRODUCTS "${CMAKE_SOURCE_DIR}/build/compile_commands.json"
    COMMENT "Copying compile_commands.json (${CMAKE_BUILD_TYPE}) to build/"
    VERBATIM
)
