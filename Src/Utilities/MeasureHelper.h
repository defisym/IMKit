#pragma once

#include <chrono>

using namespace std::chrono_literals;

struct MeasureHelper {
    using TimeStamp = decltype(std::chrono::system_clock::now());

    static TimeStamp GetNow() { return std::chrono::system_clock::now(); }

    // get us
    static auto GetIntervalUs(const TimeStamp prev) {
        return (GetNow() - prev) / 1us;
    }
    static auto GetIntervalMs(const TimeStamp prev) {
        return static_cast<float>(GetIntervalUs(prev)) / 1000.0f;
    }

    float* pTime = nullptr;
    TimeStamp start = {};
    MeasureHelper(float* p, bool bReset = true) :pTime(p), start(GetNow()) {
        if (bReset) { *p = 0.0f; }
    }
    ~MeasureHelper() { *pTime = GetIntervalMs(start); }
};