#pragma once

#include "pch.h"

// Quest3D engine entry points. highpoly.dll (and the per-channel-type DLLs,
// named after the channel's GUID) never ship import libraries usable from
// here, so every one of these is resolved at runtime by mangled name via
// Detours' DetourFindFunction. ResolveApi() must run once, early, before any
// of these pointers are dereferenced.
namespace quest3d::api
{

extern const char*(__thiscall* ChannelGroup_GetChannelGroupFileName)(A3d_ChannelGroup* self);
extern const char*(__thiscall* ChannelGroup_GetPoolName)(A3d_ChannelGroup* self);
extern int(__thiscall* ChannelGroup_GetChannelCount)(A3d_ChannelGroup* self);
extern A3d_Channel*(__thiscall* ChannelGroup_GetChannel)(A3d_ChannelGroup* self, int);
extern bool(__thiscall* ChannelGroup_GetGroupIsProtected)(A3d_ChannelGroup* self);
extern bool(__thiscall* ChannelGroup_GetReadOnly)(A3d_ChannelGroup* self);
extern void(__thiscall* ChannelGroup_SetGroupIsProtected)(A3d_ChannelGroup* self, bool newValue);
extern void(__thiscall* ChannelGroup_SetReadOnly)(A3d_ChannelGroup* self, bool newValue);
extern bool(__thiscall* ChannelGroup_SaveChannelGroup)(A3d_ChannelGroup* self, const char* fileName);
extern int(__thiscall* ChannelGroup_GetGroupIndex)(A3d_ChannelGroup* self);
extern void(__thiscall* ChannelGroup_CallStartChannel)(A3d_ChannelGroup* self);

extern const char*(__thiscall* Channel_GetChannelName)(A3d_Channel* self);
extern A3d_Channel*(__thiscall* Channel_GetChild)(A3d_Channel* self, int childNr);
extern int(__thiscall* Channel_GetChildCount)(A3d_Channel* self);
extern int(__thiscall* Channel_GetChannelIDIndexNr)(A3d_Channel* self);

extern const char*(__thiscall* StringChannel_GetString)(Aco_StringChannel* self);
extern const char*(__thiscall* StringOperator_GetString)(void* self);
extern const char*(__thiscall* Lua_GetScript)(void* self);

extern void(__thiscall* StringChannel_SetString)(Aco_StringChannel* self, const char* string);
extern BOOL(__thiscall* Lua_SetScript)(void* self, const char* string);

extern int(__thiscall* Aco_DX8_Texture_GetDesiredWidth)(Aco_DX8_Texture* self);
extern int(__thiscall* Aco_DX8_Texture_GetDesiredHeight)(Aco_DX8_Texture* self);
extern IDirect3DTexture9*(__thiscall* Aco_DX8_Texture_GetTexture)(Aco_DX8_Texture* self);
extern char*(__thiscall* Aco_DX8_Texture_GetTextureBuffer)(Aco_DX8_Texture* self);
extern int(__thiscall* Aco_DX8_Texture_GetBufferSize)(Aco_DX8_Texture* self);
extern BOOL(__thiscall* Aco_DX8_Texture_LoadTextureFromFile)(Aco_DX8_Texture* self, char* path);
extern HRESULT(__thiscall* Aco_DX8_Texture_LockTexture)(Aco_DX8_Texture* self, int level, D3DLOCKED_RECT& pLockedRect);
extern void(__thiscall* Aco_DX8_Texture_UnlockTexture)(Aco_DX8_Texture* self, int level);
extern int(__thiscall* Aco_DX8_Texture_GetMipMapLevels)(Aco_DX8_Texture* self);
extern D3DSURFACE_DESC(__thiscall* Aco_DX8_Texture_GetTextureDescription)(Aco_DX8_Texture* self, int lvl);

extern D3DMATERIAL9(__thiscall* Aco_DX8_MaterialChannel_GetMaterial)(void* self);

extern int(__thiscall* Aco_DX8_ObjectDataChannel_GetVertexCount)(Aco_DX8_ObjectDataChannel* self);
extern D3DXVECTOR3(__thiscall* Aco_DX8_ObjectDataChannel_GetVertexPosition)(Aco_DX8_ObjectDataChannel* self, DWORD nr);

extern D3DXVECTOR3(__thiscall* Aco_DX8_ObjectChannel_GetPosition)(void* self);

extern float(__thiscall* Aco_FloatChannel_GetFloat)(void* self);
extern float(__thiscall* Aco_FloatChannel_GetDefaultFloat)(void* self);
extern void(__thiscall* Aco_FloatChannel_SetFloat)(void* self, float value);

extern D3DXVECTOR3(__thiscall* Aco_VectorChannel_GetVector)(void* self);

// Resolves every pointer above via DetourFindFunction. Must be called once
// before the UI or the graph dumper touch any of them.
void ResolveApi();

} // namespace quest3d::api
