#include "InterfaceLogger.h"

#include "GUIContext/GUIContext.h"

void InterfaceLogger(const char* pID,
    Logger* pLogger, LoggerConfig* pConfig) {
    auto& bAutoScroll = pConfig->bAutoScroll;

    //// Options menu
    //if (ImGui::BeginPopup("Options")) {
    //    ImGui::Checkbox(I18N("Auto-scroll"), &bAutoScroll);
    //    ImGui::EndPopup();
    //}
    //if (ImGui::Button(I18N("Options"))) { ImGui::OpenPopup("Options"); }
    //ImGui::SameLine();

    const bool bClear = ImGui::Button(I18N("Clear"));
    ImGui::SameLine();
    const bool bCopy = ImGui::Button(I18N("Copy"));
    ImGui::SameLine();

    // Main window
    static ImGuiTextFilter filter;
    filter.Draw(I18N("Filter Log"), -100.0f);
    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        if (bClear) { pLogger->Clear(); }
        if (bCopy) { ImGui::LogToClipboard(); }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(pLogger->lines.size()));
        while (clipper.Step()) {
            for (auto pLine = pLogger->lines.crbegin() + clipper.DisplayStart;
                pLine != pLogger->lines.crbegin() + clipper.DisplayEnd;
                ++pLine) {
                if (!filter.IsActive() || filter.PassFilter(pLine->c_str())) {
                    ImGui::TextUnformatted(pLine->c_str());
                }
            }
        }
        clipper.End();
        ImGui::PopStyleVar();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (bAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();
}
