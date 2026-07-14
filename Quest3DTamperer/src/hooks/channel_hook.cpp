#include "pch.h"

#include "hooks/channel_hook.h"

#include "quest3d/state.h"

namespace hooks
{

namespace
{

void(__thiscall* TrueCallChannel)(A3d_Channel* self) = nullptr;

void __fastcall CallChannelHook(A3d_Channel* self, DWORD /*edx*/)
{
    TrueCallChannel(self);
    if(quest3d::g_engine == nullptr) {
        quest3d::g_engine = self->engine;
    }
}

} // namespace

void InstallChannelHook()
{
    TrueCallChannel = (void(__thiscall*)(A3d_Channel*))DetourFindFunction("highpoly.dll", "?CallChannel@A3d_Channel@@UAEXXZ");

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)TrueCallChannel, CallChannelHook);
    DetourTransactionCommit();
}

} // namespace hooks
