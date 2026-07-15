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

    if(!hooks::install_d3d9_hooks())
        return 0;

    // Job done - the actual hooks (hk_create_device/hk_reset/hk_end_scene/
    // hk_release/hk_wnd_proc) now run on the game's own render thread from
    // here on. Hotkey polling lives in hk_end_scene() (see d3d9_hooks.cpp)
    // instead of a loop on this thread.
    return 1;
}

} // namespace

// NOLINTNEXTLINE(readability-identifier-naming) - DllMain is a reserved name the OS looks up by exact spelling.
BOOL APIENTRY DllMain(HMODULE h_module, DWORD dw_reason, LPVOID /*lpReserved*/)
{
    DisableThreadLibraryCalls(h_module);

    switch(dw_reason) {
        case DLL_PROCESS_ATTACH:
            {
                HANDLE hook_thread = CreateThread(nullptr, 0, hook_install_thread, h_module, 0, nullptr);
                if(hook_thread != nullptr) {
                    CloseHandle(hook_thread);
                }

                DetourRestoreAfterWith();

                hooks::install_channel_hook();
                quest3d::api::resolve_api();
                break;
            }

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
