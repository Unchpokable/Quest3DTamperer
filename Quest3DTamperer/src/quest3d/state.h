#pragma once

#include "pch.h"

// Shared mutable state produced by the game-side hooks and consumed by the UI.
namespace quest3d
{

// Captured the first time A3d_Channel::CallChannel runs (see hooks/channel_hook.h).
// Null until then - the UI must handle that case.
extern EngineInterface* g_engine;

// The Audiosurf window handle, found once the D3D9 hooks are installed.
extern HWND g_gameHandle;

} // namespace quest3d
