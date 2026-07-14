#include "pch.h"

#include "hooks/channel_hook.h"
#include "hooks/d3d9_hooks.h"
#include "quest3d/api.h"

namespace
{

DWORD WINAPI HookInstallThread(LPVOID)
{
    if(kiero::init(kiero::RenderType::Auto) != kiero::Status::Success)
        return 0;

    if(kiero::getRenderType() == kiero::RenderType::D3D9) {
        hooks::InstallD3D9Hooks();
    }

    while(true) {
        // Toggle the menu.
        if(GetAsyncKeyState(VK_END) & 1) {
            hooks::g_showMenu = !hooks::g_showMenu;
        }

        // Force ImGui to reinitialize.
        if(GetAsyncKeyState(VK_INSERT) & 1) {
            hooks::g_forceImGuiReinit = true;
        }
    }

    return 1;
}

} // namespace

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
    DisableThreadLibraryCalls(hModule);

    switch(ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            CreateThread(NULL, 0, HookInstallThread, NULL, 0, NULL);

            DetourRestoreAfterWith();

            hooks::InstallChannelHook();
            quest3d::api::ResolveApi();
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
