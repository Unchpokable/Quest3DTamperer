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
} // namespace hooks

namespace
{

using reset = long(__stdcall*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
using end_scene = long(__stdcall*)(LPDIRECT3DDEVICE9);

reset o_reset = nullptr;
end_scene o_end_scene = nullptr;
WNDPROC g_wnd_proc_original = nullptr;

// The device/window ImGui is currently bound to. Null until the first
// successful hk_end_scene() call. Quest3D can tear its D3D9 device down and
// build a brand new one - not just Reset() the existing one - e.g. across
// certain fullscreen transitions, which can also mean a different focus
// window. ensure_imgui_bound() retargets both whenever either one no longer
// matches what the game just handed us.
LPDIRECT3DDEVICE9 g_bound_device = nullptr;
HWND g_bound_window = nullptr;

LRESULT __stdcall CALLBACK hk_wnd_proc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
    if(ImGui_ImplWin32_WndProcHandler(h_wnd, u_msg, w_param, l_param) && hooks::g_show_menu)
        return true;

    return CallWindowProc(g_wnd_proc_original, h_wnd, u_msg, w_param, l_param);
}

void hook_window(HWND window)
{
    g_wnd_proc_original = (WNDPROC)SetWindowLong(window, GWL_WNDPROC, (LRESULT)hk_wnd_proc);
    quest3d::g_game_handle = window;
}

// (Re)targets ImGui at whatever device/window the game is *currently*
// rendering through. Safe and cheap to call every frame: once nothing has
// changed it's just two pointer comparisons.
void ensure_imgui_bound(LPDIRECT3DDEVICE9 device)
{
    D3DDEVICE_CREATION_PARAMETERS params;
    device->GetCreationParameters(&params);

    if(device == g_bound_device && params.hFocusWindow == g_bound_window) {
        return;
    }

    if(g_bound_device == nullptr) {
        // First bind ever - nothing to tear down yet.
        ImGui::CreateContext();
    }
    else {
        // The device and/or its focus window changed under us. Tear down
        // just the backends bound to the old ones (the ImGuiContext itself
        // holds no D3D9/HWND state, so it doesn't need recreating) and
        // restore whatever WndProc we replaced on the old window.
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();

        if(g_bound_window != nullptr) {
            SetWindowLong(g_bound_window, GWL_WNDPROC, (LRESULT)g_wnd_proc_original);
        }
    }

    ImGui_ImplWin32_Init(params.hFocusWindow);
    ImGui_ImplDX9_Init(device);
    ui::init_file_dialogs();

    hook_window(params.hFocusWindow);

    g_bound_device = device;
    g_bound_window = params.hFocusWindow;
}

// Quest3D can call EndScene() more than once per frame - once per
// intermediate render target (shadow maps, reflections, other channel-driven
// render-to-texture passes) before the pass that actually targets the
// swapchain's back buffer. Drawing the overlay unconditionally on every
// EndScene() is exactly how it ends up rendered "somewhere else": onto
// whichever of those intermediate surfaces happened to be bound at the time.
bool is_rendering_to_back_buffer(LPDIRECT3DDEVICE9 device)
{
    IDirect3DSurface9* render_target = nullptr;
    if(FAILED(device->GetRenderTarget(0, &render_target)) || render_target == nullptr) {
        return false;
    }

    IDirect3DSurface9* back_buffer = nullptr;
    const bool matches = SUCCEEDED(device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer)) && back_buffer == render_target;

    render_target->Release();
    if(back_buffer != nullptr) {
        back_buffer->Release();
    }

    return matches;
}

long __stdcall hk_reset(LPDIRECT3DDEVICE9 p_device, D3DPRESENT_PARAMETERS* p_presentation_parameters)
{
    // Reset() is a method call on an *existing* device - it's never how the
    // game hands us a brand new one (that's ensure_imgui_bound()'s job, via
    // hk_end_scene). Only touch device objects if ImGui is actually bound to
    // this particular device right now.
    const bool imgui_bound_here = p_device == g_bound_device;
    if(imgui_bound_here) {
        ImGui_ImplDX9_InvalidateDeviceObjects();
    }

    const long result = o_reset(p_device, p_presentation_parameters);

    // Device objects can only be (re)created once the device has actually
    // come back. If Reset() itself failed - still lost, wrong parameters,
    // whatever - leave them invalidated; hk_end_scene() checks
    // TestCooperativeLevel() every frame and simply won't render ImGui until
    // a later Reset() succeeds and gets us back here.
    if(imgui_bound_here && SUCCEEDED(result)) {
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    return result;
}

long __stdcall hk_end_scene(LPDIRECT3DDEVICE9 p_device)
{
    // Polled here rather than on a separate thread: EndScene already runs
    // once per frame on the game's render thread, which naturally throttles
    // GetAsyncKeyState() and avoids racing hooks::g_show_menu against
    // hk_wnd_proc()/the drawing below.
    if(GetAsyncKeyState(VK_END) & 1) {
        hooks::g_show_menu = !hooks::g_show_menu;
    }

    // The device is unusable for a while around alt-tab/fullscreen
    // transitions: lost (can't render, resources can't be (re)created yet)
    // or reset-pending (the game hasn't called Reset() again yet - see
    // hk_reset() above). Either way, wait for it to come back instead of
    // touching ImGui.
    if(p_device->TestCooperativeLevel() == D3D_OK) {
        ensure_imgui_bound(p_device);

        if(hooks::g_show_menu && is_rendering_to_back_buffer(p_device)) {
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
    }

    return o_end_scene(p_device);
}

} // namespace

namespace hooks
{
void install_d3d9_hooks()
{
    // Deliberately not asserting: assert() compiles out under NDEBUG, which
    // would leave o_reset/o_end_scene null and silently swallow a bind
    // failure instead of surfacing it. A failed bind here just means that
    // particular hook never fires - the game keeps calling its own
    // Reset()/EndScene() untouched, so there's nothing else to unwind.
    if(kiero::bind(16, (void**)&o_reset, hk_reset) != kiero::Status::Success) {
        o_reset = nullptr;
    }
    if(kiero::bind(42, (void**)&o_end_scene, hk_end_scene) != kiero::Status::Success) {
        o_end_scene = nullptr;
    }
}

} // namespace hooks
