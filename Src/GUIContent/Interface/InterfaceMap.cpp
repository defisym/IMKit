#include "InterfaceMap.h"

#include "imgui.h"
#include "implot.h"

#include "Utilities/MapDownloader.h"

void InterfaceMap(TileManager* pTileManager) {
    static int renders = 0;
    static bool debug = false;
    if (ImGui::IsKeyPressed(ImGuiKey_A))
        debug = !debug;

    ImGui::SetNextWindowPos({ 0,0 });
    ImGui::SetNextWindowSize(ImGui::GetWindowSize());
    ImGui::Begin("Map", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    if (debug) {
        int wk = pTileManager->threads_working();
        int dl = pTileManager->tiles_downloaded();
        int ld = pTileManager->tiles_loaded();
        int ca = pTileManager->tiles_cached();
        int fa = pTileManager->tiles_failed();
        ImGui::Text("FPS: %.2f    Working: %d    Downloads: %d    Loads: %d    Caches: %d    Fails: %d    Renders: %d", ImGui::GetIO().Framerate, wk, dl, ld, ca, fa, renders);
    }

    ImPlotAxisFlags ax_flags = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_Foreground;
    if (ImPlot::BeginPlot("##Map", ImVec2(-1, -1), ImPlotFlags_Equal | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(NULL, NULL, ax_flags, ax_flags | ImPlotAxisFlags_Invert);
        ImPlot::SetupAxesLimits(0, 1, 0, 1);

        auto pos = ImPlot::GetPlotPos();
        auto size = ImPlot::GetPlotSize();
        auto limits = ImPlot::GetPlotLimits();
        auto& region = pTileManager->get_region(limits, size);
        renders = 0;
        if (debug) {
            float ys[] = { 1,1 };
            ImPlot::SetNextFillStyle({ 1,0,0,1 }, 0.5f);
            ImPlot::PlotShaded("##Bounds", ys, 2);
        }
        for (auto& pair : region) {
            auto& [coord, tile] = pair;
            auto [bmin, bmax] = coord.bounds();

            if (tile != nullptr) {
                auto col = debug ? ((coord.x % 2 == 0 && coord.y % 2 != 0) || (coord.x % 2 != 0 && coord.y % 2 == 0)) ? ImVec4(1, 0, 1, 1) : ImVec4(1, 1, 0, 1) : ImVec4(1, 1, 1, 1);
                ImPlot::PlotImage("##Tiles",
                    (ImTextureID)(intptr_t)tile->texture.pSrv.Get(),
                    bmin, bmax, { 0,0 }, { 1,1 }, col);
            }
            if (debug) {
                ImPlot::PlotText(coord.label().c_str(), (bmin.x + bmax.x) / 2, (bmin.y + bmax.y) / 2);
            }

            renders++;
        }

        ImPlot::PushPlotClipRect();
        static const char* label = "OpenStreetMap Contributors";
        auto label_size = ImGui::CalcTextSize(label);
        auto label_off = ImPlot::GetStyle().MousePosPadding;
        ImPlot::GetPlotDrawList()->AddText({ pos.x + label_off.x, pos.y + size.y - label_size.y - label_off.y }, IM_COL32_BLACK, label);
        ImPlot::PopPlotClipRect();

        ImPlot::EndPlot();
    }

    ImGui::End();
}