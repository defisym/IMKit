#include "InterfaceMap.h"

#include "imgui.h"
#include "implot.h"

#include "IMGuiEx/I18NInterface.h"
#include "IMGuiEx/DisplayPlot.h"

#include "Utilities/MapDownloader.h"
#include "GUIContext/Handler/MapHandler.h"

void InterfaceMap(const char* pID,
    TileManager* pTileManager, ViewParams* pViewParams,
    const std::function<void()>& extra) {
    if (ImPlot::BeginPlot(I18N(pID), ImVec2(-1, -1),
        PLOT_FLAGS | ImPlotFlags_Equal | ImPlotFlags_NoMouseText)) {
        ImPlotAxisFlags ax_flags = AXIS_FLAGS_NOMENU
            | ImPlotAxisFlags_NoLabel
            | ImPlotAxisFlags_NoTickLabels
            | ImPlotAxisFlags_NoGridLines
            | ImPlotAxisFlags_Foreground; 
        ImPlot::SetupAxes(NULL, NULL, ax_flags, ax_flags | ImPlotAxisFlags_Invert);
        ImPlot::SetupAxesLimits(0, 1, 0, 1);

        const auto& debug = pViewParams->bDebug;

        static bool bFirstRun = true;
        if (bFirstRun) {
            bFirstRun = false;
            ImPlot::SetRect({ pViewParams->xMin, pViewParams->xMax, pViewParams->yMin, pViewParams->yMax });
        }

        auto size = ImPlot::GetPlotSize();
        auto limits = ImPlot::GetPlotLimits();
        pViewParams->xMin = limits.X.Min;
        pViewParams->xMax = limits.X.Max;
        pViewParams->yMin = limits.Y.Min;
        pViewParams->yMax = limits.Y.Max;

        auto& region = pTileManager->get_region(limits, size);

        // renders needs to be reset each call
        static int renders; renders = 0;
        if (debug) {
            float ys[] = { 1,1 };
            ImPlot::SetNextFillStyle({ 1,0,0,1 }, 0.5f);
            ImPlot::PlotShaded("##Bounds", ys, 2);
        }

        for (auto& pair : region) {
            auto& [coord, tile] = pair;
            auto [bmin, bmax] = coord.bounds();

            if (tile != nullptr) {
                const auto alpha = (float)tile->FadeIn();

                auto col = debug 
                    ? ((coord.x % 2 == 0 && coord.y % 2 != 0) || (coord.x % 2 != 0 && coord.y % 2 == 0)) 
                        ? ImVec4(1, 0, 1, alpha)
                        : ImVec4(1, 1, 0, alpha)
                    : ImVec4(1, 1, 1, alpha);
                ImPlot::PlotImage("##Tiles",
                    (ImTextureID)(intptr_t)tile->texture.pSrv.Get(),
                    bmin, bmax, { 0,0 }, { 1,1 }, col);
            }
            if (debug) {
                ImPlot::PlotText(coord.label().c_str(), (bmin.x + bmax.x) / 2, (bmin.y + bmax.y) / 2);
            }

            renders++;
        }

        extra();

        ImPlot::PushPlotClipRect();
        auto pos = ImPlot::GetPlotPos();
        auto offset = ImPlot::GetStyle().MousePosPadding;

        auto addRect = [offset] (const ImVec2& leftTop, const ImVec2& textSize) {
            auto leftTopPadding = ImVec2{ leftTop.x - offset.x / 2, leftTop.y - offset.y / 2 };
            auto rightBottomPadding = ImVec2{ leftTop.x + textSize.x + offset.x / 2,
                leftTop.y + textSize.y + offset.y / 2 };
            ImPlot::GetPlotDrawList()->AddRectFilled(leftTopPadding, rightBottomPadding, IM_COL32_WHITE);
            ImPlot::GetPlotDrawList()->AddRect(leftTopPadding, rightBottomPadding, IM_COL32(0, 0, 0, 128));
            };

        {
            static const char* text = "OpenStreetMap Contributors";
            auto textSize = ImGui::CalcTextSize(text);
            ImPlot::GetPlotDrawList()->AddText({ pos.x + offset.x,
                pos.y + size.y - textSize.y - offset.y },
                IM_COL32_BLACK, text);
        }

        const auto pending = pTileManager->tiles_pending();
        if (debug || pending != 0) {
            auto text = I18NFMT("Downloading: {}", pending);
            auto textSize = ImGui::CalcTextSize(text);
            auto leftTop = ImVec2{ pos.x + size.x - textSize.x - offset.x,
                pos.y + offset.y };

            addRect(leftTop, textSize);
            ImPlot::GetPlotDrawList()->AddText(leftTop, IM_COL32_BLACK, text);
        }

        if (debug) {
            auto text = I18NFMT("Working Thread: {}, Total Downloads: {}, Total Loads: {}, Total Fails: {}, Renders: {}",
                pTileManager->threads_working(), pTileManager->tiles_downloaded(),
                pTileManager->tiles_loaded(), pTileManager->tiles_failed(), renders);
            auto textSize = ImGui::CalcTextSize(text);
            auto leftTop = ImVec2{ pos.x + size.x - textSize.x - offset.x,
                    pos.y + size.y - textSize.y - offset.y };

            addRect(leftTop, textSize);
            ImPlot::GetPlotDrawList()->AddText(leftTop, IM_COL32_BLACK, text);
        }

        ImPlot::PopPlotClipRect();

        ImPlot::EndPlot();
    }
}
