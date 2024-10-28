#pragma once

#include <algorithm>
#include <functional>

#include <_DeLib/ArithmeticTraits.h>

#include "implot/implot.h"

// setup axis with default settings
void SetupAxis(const char* xLabel = nullptr, const char* yLabel = nullptr);
// begin plot and impl default settings
bool BeginPlotEx(const char* title_id, const char* xLabel = nullptr, const char* yLabel = nullptr);
bool BeginSubPlotEx(const char* title_id, int rows, int cols);

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
    constexpr static auto THRESHOLD = 500;
    if (dataCount <= THRESHOLD && !info.xUpdater && !info.yUpdater) {
        ImPlot::PlotLine(pLabel, pData, dataCount);
        return;
    }

    struct Data {
        const T* pData = nullptr;
        int dataCount = 0;
        const PlotInfo* pInfo = nullptr;

        int offset = 1;          // read data
        double coordCoef = 1.0;  // display coord
        const CoordUpdater* pXUpdater = nullptr;
        const CoordUpdater* pYUpdater = nullptr;

        Data(const T* pD, const int sz, const PlotInfo* pI)
            :pData(pD), dataCount(sz), pInfo(pI) {
            constexpr static auto MIN_OFFSET = 1.0;
            coordCoef = (std::max)(MIN_OFFSET,
                static_cast<double>(dataCount) / static_cast<double>(THRESHOLD));
            offset = static_cast<int>(coordCoef);
            pXUpdater = pInfo->GetXUpdater();
            pYUpdater = pInfo->GetYUpdater();
        }

        ImPlotPoint GetCoord(const int idx) {
            auto pElement = pData + idx * pInfo->stride * offset;

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
        T point;
        int dataCount;
        const CoordUpdater* pXUpdater = nullptr;
    } data = { (*info.GetYUpdater())(point),dataCount,info.GetXUpdater() };

    // for single point, only display two points for a line is enough
    ImPlot::PlotLineG(pLabel, [] (int idx, void* pData) {
        const auto pPointData = static_cast<PointData*>(pData);
        idx = idx == 0 ? 0 : pPointData->dataCount;

        return ImPlotPoint{ (*pPointData->pXUpdater)(idx),
            static_cast<double>(pPointData->point) };
        }, &data, 2, info.flags | ImPlotLineFlags_Shaded);
}
