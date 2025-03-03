#include "ComponentLogger.h"

void ComponentLogger(Ctx* pCtx) {
    auto& logger = pCtx->loggerHandler.logger;
    auto& bAutoScroll = pCtx->loggerHandler.loggerParams.loggerConfig.bAutoScroll;

    // Options menu
    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-scroll", &bAutoScroll);
        ImGui::EndPopup();
    }
    if (ImGui::Button("Options")) { ImGui::OpenPopup("Options"); }
    ImGui::SameLine();

    const bool bClear = ImGui::Button("Clear");
    ImGui::SameLine();
    const bool bCopy = ImGui::Button("Copy");
    ImGui::SameLine();

    // Main window
    static ImGuiTextFilter filter;
    filter.Draw("Filter", -100.0f);
    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        if (bClear) { logger.Clear(); }
        if (bCopy) { ImGui::LogToClipboard(); }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(logger.lines.size()));
        for (auto pLine = logger.lines.crbegin(); 
            clipper.Step() && pLine != logger.lines.crend();
            ++pLine) {
            if (!filter.IsActive() || filter.PassFilter(pLine->c_str())) {
                ImGui::TextUnformatted(pLine->c_str());
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
