#pragma once

#include "pch.h"

namespace hooks
{

// Toggled by the VK_END hotkey (polled once per frame in hk_end_scene), read
// every EndScene to decide whether to draw the ImGui window at all.
extern bool g_show_menu;

// Binds kiero's D3D9 Reset/EndScene hooks and hooks the game window's WndProc.
// Must run after kiero::init() reports a D3D9 render type.
void install_d3d9_hooks();

} // namespace hooks
