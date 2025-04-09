#include "InterfaceMap.h"

#include "imgui.h"
#include "implot.h"

#include "IMGuiEx/I18NInterface.h"
#include "IMGuiEx/DisplayPlot.h"

#include "Utilities/MapDownloader.h"

void InterfaceMap(TileManager* pTileManager) {
    static int renders = 0;
    static bool debug = false;
    if (ImGui::IsKeyPressed(ImGuiKey_A)) { debug = !debug; }

    if (debug) {
        ImGui::TextUnformatted(I18NFMT("Total Downloads: {}, Total Loads: {}, Total Fails: {}, Renders: {}",
            pTileManager->tiles_downloaded(), pTileManager->tiles_loaded(), pTileManager->tiles_failed(), renders));
        ImGui::TextUnformatted(I18NFMT("Working Thread: {}, Pending: {}",
            pTileManager->threads_working(), pTileManager->tiles_pending()));
    }

    if (ImPlot::BeginPlot("##Map", ImVec2(-1, -1), 
        PLOT_FLAGS | ImPlotFlags_Equal | ImPlotFlags_NoMouseText)) {
        ImPlotAxisFlags ax_flags = AXIS_FLAGS_NOMENU
            | ImPlotAxisFlags_NoLabel
            | ImPlotAxisFlags_NoTickLabels
            | ImPlotAxisFlags_NoGridLines
            | ImPlotAxisFlags_Foreground; 
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
}
