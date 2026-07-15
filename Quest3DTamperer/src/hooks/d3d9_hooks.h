#pragma once

#include "pch.h"

namespace hooks
{

// Toggled by the VK_END hotkey (polled once per frame in hk_end_scene), read
// every EndScene to decide whether to draw the ImGui window at all.
extern bool g_show_menu;

// Resolves the live D3D9 vtable via a throwaway bootstrap device, then
// Detours-hooks IDirect3D9::CreateDevice and IDirect3DDevice9::Reset/
// EndScene/Release. Returns false if d3d9.dll isn't loaded yet or any step
// fails - callers should treat that as "hooks are not installed", not retry.
// The game window itself is hooked lazily, once a device is actually bound
// (see bind_device() in d3d9_hooks.cpp) - not here.
bool install_d3d9_hooks();

} // namespace hooks
