#include "pch.h"

#include "quest3d/api.h"

namespace quest3d::api
{

const char*(__thiscall* ChannelGroup_GetChannelGroupFileName)(A3d_ChannelGroup* self) = nullptr;
const char*(__thiscall* ChannelGroup_GetPoolName)(A3d_ChannelGroup* self) = nullptr;
int(__thiscall* ChannelGroup_GetChannelCount)(A3d_ChannelGroup* self) = nullptr;
A3d_Channel*(__thiscall* ChannelGroup_GetChannel)(A3d_ChannelGroup* self, int) = nullptr;
bool(__thiscall* ChannelGroup_GetGroupIsProtected)(A3d_ChannelGroup* self) = nullptr;
bool(__thiscall* ChannelGroup_GetReadOnly)(A3d_ChannelGroup* self) = nullptr;
void(__thiscall* ChannelGroup_SetGroupIsProtected)(A3d_ChannelGroup* self, bool newValue) = nullptr;
void(__thiscall* ChannelGroup_SetReadOnly)(A3d_ChannelGroup* self, bool newValue) = nullptr;
bool(__thiscall* ChannelGroup_SaveChannelGroup)(A3d_ChannelGroup* self, const char* fileName) = nullptr;
int(__thiscall* ChannelGroup_GetGroupIndex)(A3d_ChannelGroup* self) = nullptr;
void(__thiscall* ChannelGroup_CallStartChannel)(A3d_ChannelGroup* self) = nullptr;

const char*(__thiscall* Channel_GetChannelName)(A3d_Channel* self) = nullptr;
A3d_Channel*(__thiscall* Channel_GetChild)(A3d_Channel* self, int childNr) = nullptr;
int(__thiscall* Channel_GetChildCount)(A3d_Channel* self) = nullptr;
int(__thiscall* Channel_GetChannelIDIndexNr)(A3d_Channel* self) = nullptr;

const char*(__thiscall* StringChannel_GetString)(Aco_StringChannel* self) = nullptr;
const char*(__thiscall* StringOperator_GetString)(void* self) = nullptr;
const char*(__thiscall* Lua_GetScript)(void* self) = nullptr;

void(__thiscall* StringChannel_SetString)(Aco_StringChannel* self, const char* string) = nullptr;
BOOL(__thiscall* Lua_SetScript)(void* self, const char* string) = nullptr;

int(__thiscall* Aco_DX8_Texture_GetDesiredWidth)(Aco_DX8_Texture* self) = nullptr;
int(__thiscall* Aco_DX8_Texture_GetDesiredHeight)(Aco_DX8_Texture* self) = nullptr;
IDirect3DTexture9*(__thiscall* Aco_DX8_Texture_GetTexture)(Aco_DX8_Texture* self) = nullptr;
char*(__thiscall* Aco_DX8_Texture_GetTextureBuffer)(Aco_DX8_Texture* self) = nullptr;
int(__thiscall* Aco_DX8_Texture_GetBufferSize)(Aco_DX8_Texture* self) = nullptr;
BOOL(__thiscall* Aco_DX8_Texture_LoadTextureFromFile)(Aco_DX8_Texture* self, char* path) = nullptr;
HRESULT(__thiscall* Aco_DX8_Texture_LockTexture)(Aco_DX8_Texture* self, int level, D3DLOCKED_RECT& pLockedRect) = nullptr;
void(__thiscall* Aco_DX8_Texture_UnlockTexture)(Aco_DX8_Texture* self, int level) = nullptr;
int(__thiscall* Aco_DX8_Texture_GetMipMapLevels)(Aco_DX8_Texture* self) = nullptr;
D3DSURFACE_DESC(__thiscall* Aco_DX8_Texture_GetTextureDescription)(Aco_DX8_Texture* self, int lvl) = nullptr;

D3DMATERIAL9(__thiscall* Aco_DX8_MaterialChannel_GetMaterial)(void* self) = nullptr;

int(__thiscall* Aco_DX8_ObjectDataChannel_GetVertexCount)(Aco_DX8_ObjectDataChannel* self) = nullptr;
D3DXVECTOR3(__thiscall* Aco_DX8_ObjectDataChannel_GetVertexPosition)(Aco_DX8_ObjectDataChannel* self, DWORD nr) = nullptr;

D3DXVECTOR3(__thiscall* Aco_DX8_ObjectChannel_GetPosition)(void* self) = nullptr;

float(__thiscall* Aco_FloatChannel_GetFloat)(void* self) = nullptr;
float(__thiscall* Aco_FloatChannel_GetDefaultFloat)(void* self) = nullptr;
void(__thiscall* Aco_FloatChannel_SetFloat)(void* self, float value) = nullptr;

D3DXVECTOR3(__thiscall* Aco_VectorChannel_GetVector)(void* self) = nullptr;

void ResolveApi()
{
    ChannelGroup_GetChannelGroupFileName = (const char*(__thiscall*)(A3d_ChannelGroup*))DetourFindFunction(
        "highpoly.dll", "?GetChannelGroupFileName@A3d_ChannelGroup@@UAEPBDXZ");
    ChannelGroup_GetPoolName =
        (const char*(__thiscall*)(A3d_ChannelGroup*))DetourFindFunction("highpoly.dll", "?GetPoolName@A3d_ChannelGroup@@UAEPBDXZ");
    // TODO: this resolves to the same mangled name as GetPoolName above (likely a copy-paste mistake predating this file
    // split). Left unchanged - not touching game-interop logic during this infrastructure pass.
    ChannelGroup_GetChannelCount =
        (int(__thiscall*)(A3d_ChannelGroup*))DetourFindFunction("highpoly.dll", "?GetPoolName@A3d_ChannelGroup@@UAEPBDXZ");
    ChannelGroup_GetGroupIsProtected =
        (bool(__thiscall*)(A3d_ChannelGroup*))DetourFindFunction("highpoly.dll", "?GetGroupIsProtected@A3d_ChannelGroup@@UAE_NXZ");
    ChannelGroup_GetReadOnly =
        (bool(__thiscall*)(A3d_ChannelGroup*))DetourFindFunction("highpoly.dll", "?GetReadOnly@A3d_ChannelGroup@@UAE_NXZ");
    ChannelGroup_SetGroupIsProtected =
        (void(__thiscall*)(A3d_ChannelGroup*, bool))DetourFindFunction("highpoly.dll", "?SetGroupIsProtected@A3d_ChannelGroup@@UAEX_N@Z");
    ChannelGroup_SetReadOnly =
        (void(__thiscall*)(A3d_ChannelGroup*, bool))DetourFindFunction("highpoly.dll", "?SetReadOnly@A3d_ChannelGroup@@UAEX_N@Z");
    ChannelGroup_SaveChannelGroup = (bool(__thiscall*)(A3d_ChannelGroup*, const char*))DetourFindFunction(
        "highpoly.dll", "?SaveChannelGroup@A3d_ChannelGroup@@UAE_NPBD@Z");
    ChannelGroup_GetGroupIndex =
        (int(__thiscall*)(A3d_ChannelGroup*))DetourFindFunction("highpoly.dll", "?GetGroupIndex@A3d_ChannelGroup@@UAEHXZ");
    ChannelGroup_CallStartChannel =
        (void(__thiscall*)(A3d_ChannelGroup*))DetourFindFunction("highpoly.dll", "?CallStartChannel@A3d_ChannelGroup@@UAEXXZ");
    ChannelGroup_GetChannel = (A3d_Channel * (__thiscall*)(A3d_ChannelGroup*, int))
        DetourFindFunction("highpoly.dll", "?GetChannel@A3d_ChannelGroup@@UAEPAVA3d_Channel@@H@Z");

    Channel_GetChannelName =
        (const char*(__thiscall*)(A3d_Channel*))DetourFindFunction("highpoly.dll", "?GetChannelName@A3d_Channel@@QAEPBDXZ");
    Channel_GetChild =
        (A3d_Channel * (__thiscall*)(A3d_Channel*, int)) DetourFindFunction("highpoly.dll", "?GetChild@A3d_Channel@@QAEPAV1@H@Z");
    Channel_GetChildCount = (int(__thiscall*)(A3d_Channel*))DetourFindFunction("highpoly.dll", "?GetChildCount@A3d_Channel@@QAEHXZ");
    Channel_GetChannelIDIndexNr =
        (int(__thiscall*)(A3d_Channel*))DetourFindFunction("highpoly.dll", "?GetChannelIDIndexNr@A3d_Channel@@QAEHXZ");

    StringChannel_GetString = (const char*(__thiscall*)(Aco_StringChannel*))DetourFindFunction(
        "6E6FB247-4627-4FBE-8973-48344F23881E.dll", "?GetString@Aco_StringChannel@@UAEPBDXZ");
    StringOperator_GetString = (const char*(__thiscall*)(void*))DetourFindFunction(
        "F26BB40B-B196-4AB9-B59E-FA7C8FF436F9.dll", "?GetString@Aco_StringOperator@@UAEPBDXZ");
    Lua_GetScript =
        (const char*(__thiscall*)(void*))DetourFindFunction("6514FE12-88CF-480B-A3D8-7730C0CD23B3.dll", "?GetScript@Aco_Lua@@UAEPBDXZ");

    StringChannel_SetString = (void(__thiscall*)(Aco_StringChannel*, const char*))DetourFindFunction(
        "6E6FB247-4627-4FBE-8973-48344F23881E.dll", "?SetString@Aco_StringChannel@@UAEXPBD@Z");
    Lua_SetScript = (BOOL(__thiscall*)(void*, const char*))DetourFindFunction(
        "6514FE12-88CF-480B-A3D8-7730C0CD23B3.dll", "?SetScript@Aco_Lua@@UAE_NPBD@Z");

    Aco_DX8_Texture_GetDesiredWidth = (int(__thiscall*)(Aco_DX8_Texture*))DetourFindFunction(
        "BC052C38-2D5D-4f0c-A0CA-654D0AFC584A.dll", "?GetDesiredWidth@Aco_DX8_Texture@@UAEHXZ");
    Aco_DX8_Texture_GetDesiredHeight = (int(__thiscall*)(Aco_DX8_Texture*))DetourFindFunction(
        "BC052C38-2D5D-4f0c-A0CA-654D0AFC584A.dll", "?GetDesiredHeight@Aco_DX8_Texture@@UAEHXZ");
    Aco_DX8_Texture_GetTexture = (IDirect3DTexture9 * (__thiscall*)(Aco_DX8_Texture*))
        DetourFindFunction("BC052C38-2D5D-4f0c-A0CA-654D0AFC584A.dll", "?GetTexture@Aco_DX8_Texture@@UAEPAUIDirect3DTexture9@@XZ");
    Aco_DX8_Texture_GetTextureBuffer = (char*(__thiscall*)(Aco_DX8_Texture*))DetourFindFunction(
        "BC052C38-2D5D-4f0c-A0CA-654D0AFC584A.dll", "?GetTextureBuffer@Aco_DX8_Texture@@UAEPADXZ");
    Aco_DX8_Texture_GetBufferSize = (int(__thiscall*)(Aco_DX8_Texture*))DetourFindFunction(
        "BC052C38-2D5D-4f0c-A0CA-654D0AFC584A.dll", "?GetBufferSize@Aco_DX8_Texture@@UAEHXZ");
    Aco_DX8_Texture_LoadTextureFromFile = (BOOL(__thiscall*)(Aco_DX8_Texture*, char*))DetourFindFunction(
        "BC052C38-2D5D-4f0c-A0CA-654D0AFC584A.dll", "?LoadTextureFromFile@Aco_DX8_Texture@@UAE_NPAD@Z");
    Aco_DX8_Texture_LockTexture = (HRESULT(__thiscall*)(Aco_DX8_Texture*, int, D3DLOCKED_RECT&))DetourFindFunction(
        "BC052C38-2D5D-4f0c-A0CA-654D0AFC584A.dll", "?LockTexture@Aco_DX8_Texture@@UAEJHAAU_D3DLOCKED_RECT@@@Z");
    Aco_DX8_Texture_UnlockTexture = (void(__thiscall*)(Aco_DX8_Texture*, int))DetourFindFunction(
        "BC052C38-2D5D-4f0c-A0CA-654D0AFC584A.dll", "?UnlockTexture@Aco_DX8_Texture@@UAEXH@Z");
    Aco_DX8_Texture_GetMipMapLevels = (int(__thiscall*)(Aco_DX8_Texture*))DetourFindFunction(
        "BC052C38-2D5D-4f0c-A0CA-654D0AFC584A.dll", "?GetMipMapLevels@Aco_DX8_Texture@@UAEHXZ");
    Aco_DX8_Texture_GetTextureDescription = (D3DSURFACE_DESC(__thiscall*)(Aco_DX8_Texture*, int))DetourFindFunction(
        "BC052C38-2D5D-4f0c-A0CA-654D0AFC584A.dll", "?GetTextureDescription@Aco_DX8_Texture@@UAE?AU_D3DSURFACE_DESC@@H@Z");

    Aco_DX8_MaterialChannel_GetMaterial = (D3DMATERIAL9(__thiscall*)(void*))DetourFindFunction(
        "376A9C13-8D66-49EC-BAE5-D59BE13BC519.dll", "?GetMaterialValue@Aco_DX8_MaterialChannel@@UAEMH@Z");

    Aco_DX8_ObjectDataChannel_GetVertexCount = (int(__thiscall*)(Aco_DX8_ObjectDataChannel*))DetourFindFunction(
        "21A8923D-B908-4104-AE88-B6718D8A8678.dll", "?GetVertexCount@Aco_DX8_ObjectDataChannel@@UAEHXZ");
    Aco_DX8_ObjectDataChannel_GetVertexPosition = (D3DXVECTOR3(__thiscall*)(Aco_DX8_ObjectDataChannel*, DWORD))DetourFindFunction(
        "21A8923D-B908-4104-AE88-B6718D8A8678.dll", "?GetVertexPosition@Aco_DX8_ObjectDataChannel@@UAE?AUD3DXVECTOR3@@K@Z");

    Aco_DX8_ObjectChannel_GetPosition = (D3DXVECTOR3(__thiscall*)(void*))DetourFindFunction(
        "10C20C0A-7A55-4084-8676-95E5699BCEC2.dll", "?GetPosition@Aco_DX8_ObjectChannel@@UAE?AUD3DXVECTOR3@@XZ");

    Aco_FloatChannel_GetFloat =
        (float(__thiscall*)(void*))DetourFindFunction("BE69CCC4-CFC1-4362-AC81-767D199BBFC3.dll", "?GetFloat@Aco_FloatChannel@@UAEMXZ");
    Aco_FloatChannel_GetDefaultFloat = (float(__thiscall*)(void*))DetourFindFunction(
        "BE69CCC4-CFC1-4362-AC81-767D199BBFC3.dll", "?GetDefaultFloat@Aco_FloatChannel@@UAEMXZ");
    Aco_FloatChannel_SetFloat = (void(__thiscall*)(void*, float))DetourFindFunction(
        "BE69CCC4-CFC1-4362-AC81-767D199BBFC3.dll", "?SetFloat@Aco_FloatChannel@@UAEXM@Z");

    Aco_VectorChannel_GetVector = (D3DXVECTOR3(__thiscall*)(void*))DetourFindFunction(
        "9D045960-EAC2-4C40-9BBF-10F32F7FA305.dll", "?GetVector@Aco_VectorChannel@@UAE?AUD3DXVECTOR3@@XZ");
}

} // namespace quest3d::api
