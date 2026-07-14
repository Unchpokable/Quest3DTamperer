#include "pch.h"

#include "hooks/d3d9_hooks.h"

#include "quest3d/state.h"
#include "ui/tamperer_window.h"

// imgui_impl_win32.h intentionally omits this declaration (to avoid forcing
// <windows.h> on every consumer) and asks callers to copy it in manually.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace hooks
{

namespace
{

using Reset = long(__stdcall*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
using EndScene = long(__stdcall*)(LPDIRECT3DDEVICE9);

Reset oReset = nullptr;
EndScene oEndScene = nullptr;
WNDPROC g_wndProcOriginal = nullptr;
bool g_imguiInitialized = false;

LRESULT __stdcall CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) && g_showMenu)
        return true;

    return CallWindowProc(g_wndProcOriginal, hWnd, uMsg, wParam, lParam);
}

long __stdcall hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    long result = oReset(pDevice, pPresentationParameters);
    ImGui_ImplDX9_CreateDeviceObjects();

    return result;
}

long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
    if(g_forceImGuiReinit) {
        g_imguiInitialized = false;
        g_forceImGuiReinit = false;
    }

    if(!g_imguiInitialized) {
        D3DDEVICE_CREATION_PARAMETERS params;
        pDevice->GetCreationParameters(&params);

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(params.hFocusWindow);
        ImGui_ImplDX9_Init(pDevice);

        ui::init_file_dialogs();

        g_imguiInitialized = true;
    }

    if(g_showMenu) {
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

    return oEndScene(pDevice);
}

} // namespace

bool g_showMenu = false;
bool g_forceImGuiReinit = false;

void InstallD3D9Hooks()
{
    assert(kiero::bind(16, (void**)&oReset, hkReset) == kiero::Status::Success);
    assert(kiero::bind(42, (void**)&oEndScene, hkEndScene) == kiero::Status::Success);

    quest3d::g_gameHandle = FindWindow(NULL, L"Audiosurf");

    g_wndProcOriginal = (WNDPROC)SetWindowLong(quest3d::g_gameHandle, GWL_WNDPROC, (LRESULT)hkWndProc);
}

} // namespace hooks
