#pragma once

#include <algorithm>
#include <functional>

#include <ArithmeticTraits.h>

#include "macro.h"
#include "implot/implot.h"

// setup axis with default settings
void SetupAxis(const char* xLabel = nullptr, const char* yLabel = nullptr);
// begin plot and impl default settings
bool BeginPlotEx(const char* pTitle, const char* xLabel = nullptr, const char* yLabel = nullptr);
// it's okay to call `BeginPlotEx` inside this, as the size of plot
// will be updated by the parent plot automatically
bool BeginSubPlotEx(const char* pTitle, int rows, int cols);

using CoordUpdater = std::function<double(const double)>;

struct PlotInfo {
    int stride = 1;
    CoordUpdater xUpdater = nullptr;
    CoordUpdater yUpdater = nullptr;
    ImPlotLineFlags flags = ImPlotLineFlags_None;

    [[nodiscard]] static const CoordUpdater* GetUpdater(const CoordUpdater& updater) {
        static const CoordUpdater DEFAULT_UPDATER = [] (const double coord) { return coord; };
        return updater ? &updater : &DEFAULT_UPDATER;
    }

    [[nodiscard]] const CoordUpdater* GetXUpdater() const { return GetUpdater(xUpdater); }
    [[nodiscard]] const CoordUpdater* GetYUpdater() const { return GetUpdater(yUpdater); }
};

// display elements
template<ArithmeticConcept T>
inline void DisplayPlot(const char* pLabel,
    const T* pData, int dataCount, const PlotInfo& info = {}) {    
    if (pData == nullptr) { return; }
    
    constexpr static auto THRESHOLD = OPTIMIZE_PLOT_DSIPLAY_THRESHOLD;

    struct Data {
        const T* pData = nullptr;
        int dataCount = 0;
        const PlotInfo* pInfo = nullptr;

        double coordCoef = 1.0;  // display coord
        const CoordUpdater* pXUpdater = nullptr;
        const CoordUpdater* pYUpdater = nullptr;

        Data(const T* pD, const int sz, const PlotInfo* pI)
            :pData(pD), dataCount(sz), pInfo(pI) {
            constexpr static auto MIN_OFFSET = 1.0;
            coordCoef = (std::max)(MIN_OFFSET,
                static_cast<double>(dataCount) / static_cast<double>(THRESHOLD));
            pXUpdater = pInfo->GetXUpdater();
            pYUpdater = pInfo->GetYUpdater();
        }

        ImPlotPoint GetCoord(const int idx) {
            auto pElement = pData + static_cast<size_t>(std::floor(idx * coordCoef)) * pInfo->stride;

            return { (*pXUpdater)(idx * coordCoef),
                (*pYUpdater)(static_cast<double>(*pElement)) };
        }
    } plotData = { pData,dataCount,&info };

    ImPlot::PlotLineG(pLabel,
        [] (int idx, void* pUserData) {
            return static_cast<Data*>(pUserData)->GetCoord(idx);
        },
        &plotData, (std::min)(dataCount, THRESHOLD), info.flags);
}

template<typename T>
concept PointData = ArithmeticConcept<T> && !std::is_pointer_v<T>;

// display one element
template<PointData T>
inline void DisplayPlot(const char* pLabel,
    const T point, const int dataCount, const PlotInfo& info = {}) {
    struct PointData {
        double point;
        int dataCount;
        const CoordUpdater* pXUpdater = nullptr;
    } data = { (*info.GetYUpdater())(point),
        dataCount,info.GetXUpdater() };

    // for single point, only display two points for a line is enough
    ImPlot::PlotLineG(pLabel, [] (int idx, void* pData) {
        const auto pPointData = static_cast<PointData*>(pData);
        idx = idx == 0 ? 0 : pPointData->dataCount;

        return ImPlotPoint{ (*pPointData->pXUpdater)(idx), pPointData->point };
        }, &data, 2, info.flags | ImPlotLineFlags_Shaded);
}
