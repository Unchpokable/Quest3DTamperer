#include "pch.h"

#include "hooks/channel_hook.h"
#include "hooks/d3d9_hooks.h"

#include "quest3d/api.h"
#include "quest3d/state.h"

namespace
{

DWORD WINAPI hook_install_thread(LPVOID self_instance)
{
    quest3d::g_self_dll_handle = reinterpret_cast<HMODULE>(self_instance);

    if(kiero::init(kiero::RenderType::Auto) != kiero::Status::Success)
        return 0;

    if(kiero::getRenderType() == kiero::RenderType::D3D9) {
        hooks::install_d3d9_hooks();
    }

    // TODO: TOO BAD! HANDLE KEYBOARD AT EndScene, NOT OUTSIDE MAIN THREAD!
    while(true) {
        if(GetAsyncKeyState(VK_END) & 1) {
            hooks::g_show_menu = !hooks::g_show_menu;
        }

        // Force ImGui to reinitialize.
        // TODO: TOOOOOOOOOOOOO BAAAAAAAAAAAAAAAAAAAAAD!
        if(GetAsyncKeyState(VK_INSERT) & 1) {
            hooks::g_force_im_gui_reinit = true;
        }
    }

    return 1;
}

} // namespace

// NOLINTNEXTLINE(readability-identifier-naming) - DllMain is a reserved name the OS looks up by exact spelling.
BOOL APIENTRY DllMain(HMODULE h_module, DWORD dw_reason, LPVOID /*lpReserved*/)
{
    DisableThreadLibraryCalls(h_module);

    switch(dw_reason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(h_module);
            ::CreateThread(nullptr, 0, hook_install_thread, h_module, 0, nullptr);

            DetourRestoreAfterWith();

            hooks::install_channel_hook();
            quest3d::api::resolve_api();
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
