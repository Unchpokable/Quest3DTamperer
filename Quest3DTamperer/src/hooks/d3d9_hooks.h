#pragma once

#include "pch.h"

namespace hooks
{

// Toggled by the VK_END hotkey in main.cpp, read every EndScene to decide
// whether to draw the ImGui window at all.
extern bool g_showMenu;

// Set by the VK_INSERT hotkey in main.cpp to force ImGui to reinitialize on
// the next EndScene.
extern bool g_forceImGuiReinit;

// Binds kiero's D3D9 Reset/EndScene hooks and hooks the game window's WndProc.
// Must run after kiero::init() reports a D3D9 render type.
void InstallD3D9Hooks();

} // namespace hooks
