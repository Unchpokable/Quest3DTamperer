#include "pch.h"

#include "ui/tamperer_window.h"

#include "graph/channel_graph_dump.h"

#include "quest3d/api.h"
#include "quest3d/state.h"

#include "util/clipboard.h"
#include "util/text_convert.h"

using namespace quest3d::api;

namespace ui
{

namespace
{

ImGui::FileBrowser save_group_file_dialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir);
ImGui::FileBrowser save_texture_file_dialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir);
ImGui::FileBrowser save_graph_file_dialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir);
ImGui::FileBrowser load_group_file_dialog(0);
ImGui::FileBrowser load_texture_file_dialog(0);

int channel_group_to_use = 0;
int channel_in_group_to_use = 1;
int mipmap_level_to_use = 0;
char new_group_name[128] = "New Group";
char new_text[1024] = "New Text";
bool texture_locked = false;
bool preview_texture = true;
float new_float = 0;
char new_script[20000] = "-- Script here!";

} // namespace

void init_file_dialogs()
{
    save_group_file_dialog.SetTitle("Save channel group");
    save_group_file_dialog.SetTypeFilters({ ".cgr" });

    load_group_file_dialog.SetTitle("Load channel group");
    load_group_file_dialog.SetTypeFilters({ ".cgr" });

    save_texture_file_dialog.SetTitle("Save texture");
    save_texture_file_dialog.SetTypeFilters({ ".tga", ".png", ".jpg" });

    load_texture_file_dialog.SetTitle("Load texture");
    load_texture_file_dialog.SetTypeFilters({ ".tga", ".png", ".jpg" });

    save_graph_file_dialog.SetTitle("Save DOT Digraph");
    save_graph_file_dialog.SetTypeFilters({ ".gv" });
}

void draw_tamper_window()
{
    A3d_ChannelGroup* group = nullptr;
    A3d_Channel* channel = nullptr;
    std::string channel_guid;

    ImGui::Begin("Quest3DTamperer");
    ImGui::Spacing();
    if(quest3d::g_engine != nullptr) {
        if(ImGui::CollapsingHeader("Channel groups")) {
            ImGui::Spacing();

            ImGui::InputText("Pool name for new group", new_group_name, IM_ARRAYSIZE(new_group_name));
            if(ImGui::Button("Load channel group file")) {
                load_group_file_dialog.Open();
            }
            ImGui::Spacing();

            ImGui::InputInt("A3d_ChannelGroup to use", &channel_group_to_use, 1, 10);
            ImGui::Spacing();

            group = quest3d::g_engine->GetChannelGroup(channel_group_to_use);
            if(group != nullptr) {
                ImGui::Text("Info of current channel group:");
                ImGui::Text(ChannelGroup_GetPoolName(group));
                ImGui::Text(ChannelGroup_GetChannelGroupFileName(group));
                ImGui::Text("Is group protected: %s", ChannelGroup_GetGroupIsProtected ? "true" : "false");
                ImGui::Text("Is group read-only: %s", ChannelGroup_GetGroupIsProtected ? "true" : "false");
                if(ImGui::Button("Save group without protection")) {
                    ChannelGroup_SetReadOnly(group, false);
                    ChannelGroup_SetGroupIsProtected(group, false);
                    save_group_file_dialog.Open();
                }
                ImGui::Spacing();

                ImGui::Text("Group has %i channels", ChannelGroup_GetChannelCount(group));
                ImGui::InputInt("Channel in group to get", &channel_in_group_to_use, 1, 10);
                channel = ChannelGroup_GetChannel(group, channel_in_group_to_use);
                if(channel != nullptr) {
                    ImGui::Text(Channel_GetChannelName(channel));

                    ImGui::Text(channel->GetChannelType().name);

                    OLECHAR* guid_string;
                    StringFromCLSID(channel->GetChannelType().guid, &guid_string);
                    std::wstring wstring = std::wstring(guid_string);
                    std::string stdstring_guid = util::utf8_encode(wstring);
                    channel_guid = stdstring_guid;

                    ImGui::Text(stdstring_guid.c_str());

                    if(ImGui::Button("Save as DOT Digraph")) {
                        save_graph_file_dialog.Open();
                    }

                    if(strstr(stdstring_guid.c_str(), "6E6FB247-4627")) {
                        Aco_StringChannel* string_channel(reinterpret_cast<Aco_StringChannel*>(channel));

                        ImGui::Text("Text in channel: %s", StringChannel_GetString(string_channel));

                        if(ImGui::Button("Copy to clipboard")) {
                            util::copy_to_clipboard(quest3d::g_game_handle, StringChannel_GetString((Aco_StringChannel*)channel));
                        }
                        ImGui::Spacing();

                        ImGui::InputText("New text to set", new_text, IM_ARRAYSIZE(new_text));
                        if(ImGui::Button("Set text")) {
                            StringChannel_SetString((Aco_StringChannel*)channel, new_text);
                        }
                    }

                    if(strstr(stdstring_guid.c_str(), "F26BB40B-B196")) {
                        ImGui::Text("Text in channel: %s", StringOperator_GetString(channel));
                    }

                    if(strstr(stdstring_guid.c_str(), "6514FE12-88CF")) {
                        ImGui::Text("Script: \n%s", Lua_GetScript(channel));

                        if(ImGui::Button("Copy to clipboard")) {
                            util::copy_to_clipboard(quest3d::g_game_handle, Lua_GetScript(channel));
                        }
                        ImGui::Spacing();

                        ImGui::InputTextMultiline("New script", new_script, IM_ARRAYSIZE(new_script));
                        if(ImGui::Button("Set script")) {
                            Lua_SetScript(channel, new_script);
                        }
                    }

                    if(strstr(stdstring_guid.c_str(), "BC052C38-2D5D")) {
                        Aco_DX8_Texture* texture = (Aco_DX8_Texture*)channel;
                        ImGui::Text("Mipmap level count: %d", Aco_DX8_Texture_GetMipMapLevels(texture));
                        ImGui::InputInt("Select Mipmap level", &mipmap_level_to_use, 1, 10);
                        ImGui::Checkbox("Enable preview", &preview_texture);

                        // You should turn off the preview when swapping textures.
                        // Else the game WILL crash.
                        if(!texture_locked && preview_texture) {
                            IDirect3DTexture9* d3d_texture = Aco_DX8_Texture_GetTexture(texture);
                            D3DSURFACE_DESC description = Aco_DX8_Texture_GetTextureDescription(texture, mipmap_level_to_use);
                            ImGui::Text("Texture size: %dx%d", description.Width, description.Height);
                            ImGui::Image((void*)d3d_texture, ImVec2(description.Width, description.Height));
                        }

                        if(ImGui::Button("Save texture")) {
                            save_texture_file_dialog.Open();
                        }
                        if(ImGui::Button("Load texture")) {
                            load_texture_file_dialog.Open();
                        }
                    }

                    if(strstr(stdstring_guid.c_str(), "376A9C13-8D66")) {
                        D3DMATERIAL9 material = Aco_DX8_MaterialChannel_GetMaterial(channel);
                        ImGui::Text("Power: %f", material.Power);

                        ImVec4 specular = ImVec4(material.Specular.r, material.Specular.g, material.Specular.b, material.Specular.a);
                        ImGui::TextColored(specular, "Specular color");
                        ImVec4 emissive = ImVec4(material.Emissive.r, material.Emissive.g, material.Emissive.b, material.Emissive.a);
                        ImGui::TextColored(emissive, "Emissive color");
                        ImVec4 ambient = ImVec4(material.Ambient.r, material.Ambient.g, material.Ambient.b, material.Ambient.a);
                        ImGui::TextColored(ambient, "Ambient color");
                        ImVec4 diffuse = ImVec4(material.Diffuse.r, material.Diffuse.g, material.Diffuse.b, material.Diffuse.a);
                        ImGui::TextColored(diffuse, "Diffuse color");
                    }

                    if(strstr(stdstring_guid.c_str(), "21A8923D-B908")) {
                        Aco_DX8_ObjectDataChannel* object_data = (Aco_DX8_ObjectDataChannel*)channel;
                        ImGui::Text("Vertex count: %d", Aco_DX8_ObjectDataChannel_GetVertexCount(object_data));
                    }

                    if(strstr(stdstring_guid.c_str(), "10C20C0A-7A55")) {
                        // Aco_DX8_ObjectChannel_GetPosition currently results in an access
                        // violation - not wired up to the UI until that's understood.
                    }

                    if(strstr(stdstring_guid.c_str(), "BE69CCC4-CFC1")) {
                        ImGui::Text("Float value: %f", Aco_FloatChannel_GetFloat(channel));
                        ImGui::Text("Default value: %f", Aco_FloatChannel_GetDefaultFloat(channel));
                        ImGui::InputFloat("New float", &new_float);
                        if(ImGui::Button("Set float")) {
                            Aco_FloatChannel_SetFloat(channel, new_float);
                        }
                    }

                    if(strstr(stdstring_guid.c_str(), "9D045960-EAC2")) {
                        // Aco_VectorChannel_GetVector is resolved but not wired up to the UI yet.
                    }

                    ImGui::Spacing();
                }
            }
            else {
                ImGui::Text("Group not found!");
            }
        }
    }
    else {
        ImGui::Text("Please perform any action in-game.\nThis is needed to get the pointer to the EngineInterface,\nwhich is neccessary "
                    "for Quest3DTamperer to work.");
    }
    ImGui::End();

    save_group_file_dialog.Display();
    load_group_file_dialog.Display();
    save_texture_file_dialog.Display();
    load_texture_file_dialog.Display();
    save_graph_file_dialog.Display();

    if(save_group_file_dialog.HasSelected()) {
        ChannelGroup_SaveChannelGroup(group, save_group_file_dialog.GetSelected().string().c_str());
        save_group_file_dialog.ClearSelected();
    }

    if(load_group_file_dialog.HasSelected()) {
        A3d_ChannelGroup* new_group =
            quest3d::g_engine->LoadChannelGroup(load_group_file_dialog.GetSelected().string().c_str(), new_group_name);
        if(new_group != nullptr) {
            ChannelGroup_CallStartChannel(new_group);
        }
        load_group_file_dialog.ClearSelected();
    }

    if(save_texture_file_dialog.HasSelected()) {
        std::ofstream binary_file(save_texture_file_dialog.GetSelected().string().c_str(), std::ios::out | std::ios::binary);
        if(binary_file.is_open()) {
            Aco_DX8_Texture* texture = (Aco_DX8_Texture*)channel;
            char* data = Aco_DX8_Texture_GetTextureBuffer(texture);
            int size = Aco_DX8_Texture_GetBufferSize(texture);
            binary_file.write(data, size);
        }
        load_group_file_dialog.ClearSelected();
    }

    if(load_texture_file_dialog.HasSelected()) {
        if(strstr(channel_guid.c_str(), "BC052C38-2D5D")) {
            texture_locked = true;
            Aco_DX8_Texture* texture = (Aco_DX8_Texture*)channel;
            D3DSURFACE_DESC description = Aco_DX8_Texture_GetTextureDescription(texture, mipmap_level_to_use);
            D3DLOCKED_RECT locked_rect;
            locked_rect.pBits = (void*)Aco_DX8_Texture_GetTextureBuffer(texture);
            locked_rect.Pitch = description.Width * 4;

            Aco_DX8_Texture_LockTexture(texture, mipmap_level_to_use, locked_rect);
            Aco_DX8_Texture_LoadTextureFromFile(texture, (char*)load_texture_file_dialog.GetSelected().string().c_str());
            Aco_DX8_Texture_UnlockTexture(texture, mipmap_level_to_use);
            texture_locked = false;
        }
        load_group_file_dialog.ClearSelected();
    }

    if(save_graph_file_dialog.HasSelected()) {
        UGraphviz::Graph channel_graph(ChannelGroup_GetPoolName(group), true);
        graph::write_channel_graph(group, &channel_graph);

        std::ofstream file(save_graph_file_dialog.GetSelected().string().c_str(), std::ofstream::trunc);
        std::string dot_source = channel_graph.Dump();
        file << dot_source.c_str();
        file.close();

        save_graph_file_dialog.ClearSelected();
    }
}

} // namespace ui
