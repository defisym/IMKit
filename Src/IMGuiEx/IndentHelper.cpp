#include "IndentHelper.h"

#include "imgui.h"

IndentHelper::IndentHelper(const float i):indent(i) {
    ImGui::Indent(indent);
}

IndentHelper::~IndentHelper() { ImGui::Unindent(indent); }
