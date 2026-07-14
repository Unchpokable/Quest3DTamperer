# Dear ImGui (https://github.com/ocornut/imgui) - vendored as-is, core +
# win32/dx9 backends only (the ones this project actually uses).

add_library(imgui STATIC
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    backends/imgui_impl_win32.cpp
    backends/imgui_impl_dx9.cpp
)

target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(imgui PUBLIC dxsdk)
