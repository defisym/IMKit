#pragma once

#include <algorithm>
#include <functional>

#include "./../implot/implot.h"

using CoordUpdater = std::function<double(const double)>;

template<typename T>
inline void DisplayPlot(const char* pLabel,
    const T * pData, int dataCount, const int stride = 1,
    const CoordUpdater & xUpdater = nullptr,
    const CoordUpdater & yUpdater = nullptr) {
    constexpr auto threshold = 500;
    constexpr auto minOffset = 1;
    static const CoordUpdater defaultUpdater = [] (const double coord) { return coord; };

    if (dataCount <= threshold && !xUpdater && !yUpdater) {
        ImPlot::PlotLine(pLabel, pData, dataCount);
    }

    struct Data {
        const T* pData = nullptr;
        int stride = 1;
        int offset = 1;

        const CoordUpdater* pXUpdater = nullptr;
        const CoordUpdater* pYUpdater = nullptr;
    };

    Data data = { pData, stride,
        (std::max)(minOffset, dataCount / threshold),
        xUpdater ? &xUpdater : &defaultUpdater,
        yUpdater ? &yUpdater : &defaultUpdater };

    ImPlot::PlotLineG(pLabel,
        [] (int idx, void* user_data) {
        auto pData = static_cast<Data*>(user_data);
        auto pElement = pData->pData + pData->stride * pData->offset * idx;

        ImPlotPoint point = { (*pData->pXUpdater)(static_cast<double>(idx * pData->offset)),
            (*pData->pXUpdater)(static_cast<double>(*pElement)) };

        return point;
        },
        &data, (std::min)(dataCount, threshold), ImPlotLineFlags_None);
};

