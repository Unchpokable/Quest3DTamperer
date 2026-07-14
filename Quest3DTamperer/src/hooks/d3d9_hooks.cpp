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
std::once_flag g_imgui_init_once;

LRESULT __stdcall CALLBACK hk_wnd_proc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
    if(ImGui_ImplWin32_WndProcHandler(h_wnd, u_msg, w_param, l_param) && hooks::g_show_menu)
        return true;

    return CallWindowProc(g_wnd_proc_original, h_wnd, u_msg, w_param, l_param);
}

long __stdcall hk_reset(LPDIRECT3DDEVICE9 p_device, D3DPRESENT_PARAMETERS* p_presentation_parameters)
{
    // No ImGui context yet (hk_end_scene hasn't run once successfully) means
    // there's nothing to invalidate/recreate device objects for.
    const bool imgui_ready = ImGui::GetCurrentContext() != nullptr;
    if(imgui_ready) {
        ImGui_ImplDX9_InvalidateDeviceObjects();
    }

    const long result = o_reset(p_device, p_presentation_parameters);

    // Device objects can only be (re)created once the device has actually
    // come back. If Reset() itself failed - still lost, wrong parameters,
    // whatever - leave them invalidated; hk_end_scene() checks
    // TestCooperativeLevel() every frame and simply won't render ImGui until
    // a later Reset() succeeds and gets us back here.
    if(imgui_ready && SUCCEEDED(result)) {
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
    // touching ImGui - device objects may not exist right now.
    if(p_device->TestCooperativeLevel() == D3D_OK) {
        std::call_once(g_imgui_init_once, [p_device] {
            D3DDEVICE_CREATION_PARAMETERS params;
            p_device->GetCreationParameters(&params);

            ImGui::CreateContext();
            ImGui_ImplWin32_Init(params.hFocusWindow);
            ImGui_ImplDX9_Init(p_device);

            ui::init_file_dialogs();
        });

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

    quest3d::g_game_handle = FindWindow(nullptr, L"Audiosurf");
    if(quest3d::g_game_handle == nullptr) {
        return;
    }

    g_wnd_proc_original = (WNDPROC)SetWindowLong(quest3d::g_game_handle, GWL_WNDPROC, (LRESULT)hk_wnd_proc);
}

} // namespace hooks
