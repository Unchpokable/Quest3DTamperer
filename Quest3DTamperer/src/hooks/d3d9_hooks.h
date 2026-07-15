#pragma once

#include "pch.h"

namespace hooks
{

// Toggled by the VK_END hotkey (polled once per frame in hk_end_scene), read
// every EndScene to decide whether to draw the ImGui window at all.
extern bool g_show_menu;

// Binds kiero's D3D9 Reset/EndScene hooks. Must run after kiero::init()
// reports a D3D9 render type. The game window itself is hooked lazily, once
// hk_end_scene() actually has a live device to ask for its real focus window
// (see ensure_imgui_bound() in d3d9_hooks.cpp) - not here.
void install_d3d9_hooks();

} // namespace hooks
