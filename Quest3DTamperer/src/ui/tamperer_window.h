#pragma once

#include "pch.h"

namespace ui
{

// Sets titles/type filters on the file browser dialogs. Call once, after
// ImGui itself has been initialized.
void init_file_dialogs();

// Draws the whole Quest3DTamperer window plus its file dialogs, and handles
// dialog results (saving/loading channel groups, textures, DOT dumps). Call
// once per frame, between ImGui::NewFrame() and ImGui::EndFrame().
void draw_tamper_window();

} // namespace ui
