#include "ComponentWindow.h"

#include <imgui.h>

// NOLINT(cppcoreguidelines-special-member-functions)
ComponentWindow::ComponentWindow(const char* pName) {
    // ref: https://stackoverflow.com/questions/74954814/remove-the-parent-default-window-in-imgui
    // place the next window in the top left corner (0,0), resize to parent window size
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    // hide it
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;

    // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Begin(pName, nullptr, window_flags);
}

ComponentWindow::~ComponentWindow() { ImGui::End(); }
