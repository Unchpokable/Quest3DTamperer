#include "pch.h"

#include "hooks/d3d9_hooks.h"

#include "quest3d/state.h"

#include "ui/tamperer_window.h"

// imgui_impl_win32.h intentionally omits this declaration (to avoid forcing
// <windows.h> on every consumer) and asks callers to copy it in manually.
// NOTE: must keep this exact name - it's the real symbol imgui exports, not
// ours to rename to match our naming convention.
// NOLINTNEXTLINE(readability-identifier-naming)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param);

namespace hooks
{
bool g_show_menu = false;
bool g_force_im_gui_reinit = false;
} // namespace hooks

namespace
{

using reset = long(__stdcall*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
using end_scene = long(__stdcall*)(LPDIRECT3DDEVICE9);

reset o_reset = nullptr;
end_scene o_end_scene = nullptr;
WNDPROC g_wnd_proc_original = nullptr;
bool g_imgui_initialized = false;

LRESULT __stdcall CALLBACK hk_wnd_proc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
    if(ImGui_ImplWin32_WndProcHandler(h_wnd, u_msg, w_param, l_param) && hooks::g_show_menu)
        return true;

    return CallWindowProc(g_wnd_proc_original, h_wnd, u_msg, w_param, l_param);
}

long __stdcall hk_reset(LPDIRECT3DDEVICE9 p_device, D3DPRESENT_PARAMETERS* p_presentation_parameters)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    long result = o_reset(p_device, p_presentation_parameters);
    ImGui_ImplDX9_CreateDeviceObjects();

    return result;
}

long __stdcall hk_end_scene(LPDIRECT3DDEVICE9 p_device)
{
    // TODO: REMOVE!! ONLY REINIT IMGUI IN RESET WHEN IDirecti3DDevice9::CooperativeLevel is DEVICE_LOST!
    if(hooks::g_force_im_gui_reinit) {
        g_imgui_initialized = false;
        hooks::g_force_im_gui_reinit = false;
    }

    if(!g_imgui_initialized) {
        D3DDEVICE_CREATION_PARAMETERS params;
        p_device->GetCreationParameters(&params);

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(params.hFocusWindow);
        ImGui_ImplDX9_Init(p_device);

        ui::init_file_dialogs();

        g_imgui_initialized = true;
    }

    if(hooks::g_show_menu) {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Audiosurf has its own cursor which renders below the ImGui window,
        // and it ignores all other windows above it and registers clicks
        // through them. No workaround for this (yet) - this is
        // Audiosurf/Quest3D's fault, not ours.
        ImGui::GetIO().MouseDrawCursor = true;

        ui::draw_tamper_window();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    return o_end_scene(p_device);
}

} // namespace

namespace hooks
{
void install_d3d9_hooks()
{
    assert(kiero::bind(16, (void**)&o_reset, hk_reset) == kiero::Status::Success);
    assert(kiero::bind(42, (void**)&o_end_scene, hk_end_scene) == kiero::Status::Success);

    quest3d::g_game_handle = FindWindow(nullptr, L"Audiosurf");

    g_wnd_proc_original = (WNDPROC)SetWindowLong(quest3d::g_game_handle, GWL_WNDPROC, (LRESULT)hk_wnd_proc);
}

} // namespace hooks
