#pragma once

#include "ORL.h"

#include <type_traits>

constexpr int DEFAULT_FILTER_ORDER = 4;
constexpr int MAX_FILTER_ORDER = 32;

struct FilterParamBase {
    bool bEnable = false;
    bool bUpdate = false;

    // sample rate is updated automatically according
    // to the device param
    //double sampleRate = 0.0;
    int order = DEFAULT_FILTER_ORDER;
    // zero phase filter
    // some filter has no different if enabled
    bool bZeroPhase = false;
};

constexpr double DEFAULT_LOWPASS_CUTOFF_FREQUENCY = 800.0;

struct LowPassFilterParam :FilterParamBase {
    int filterStopFrequency = static_cast<int>(DEFAULT_LOWPASS_CUTOFF_FREQUENCY);
};

constexpr double DEFAULT_HIGHPASS_CUTOFF_FREQUENCY = 2.0;

struct HighPassFilterParam :FilterParamBase {
    int filterStopFrequency = static_cast<int>(DEFAULT_HIGHPASS_CUTOFF_FREQUENCY);
};

constexpr size_t DEFAULT_MEAN_RADIUS = 3;

struct MeanFilterParam :FilterParamBase {
    int radius = DEFAULT_MEAN_RADIUS;
};

template<>
struct std::hash<FilterParamBase> {
    std::size_t operator()(FilterParamBase const& s) const noexcept;
};

template<>
struct std::hash<LowPassFilterParam> {
    std::size_t operator()(LowPassFilterParam const& s) const noexcept;
};

template<>
struct std::hash<HighPassFilterParam> {
    std::size_t operator()(HighPassFilterParam const& s) const noexcept;
};

template<>
struct std::hash<MeanFilterParam> {
    std::size_t operator()(MeanFilterParam const& s) const noexcept;
};
