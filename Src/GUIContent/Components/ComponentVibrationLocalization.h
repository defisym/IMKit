// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
#pragma once

#include "ComponentBase.h"

struct ComponentVibrationLocalization  {
    ComponentVibrationLocalization() = default;
    virtual ~ComponentVibrationLocalization() = default;

    using ReturnType = decltype(std::make_tuple(static_cast<OTDRProcessValueType*>(nullptr), static_cast<size_t>(0)));

    virtual ReturnType MovingAverage(OTDRProcessValueType* pBuffer,
        const size_t frameCount, const size_t frameSize,
        const size_t maRange);
    virtual ReturnType MovingDifference(OTDRProcessValueType* pBuffer,
        const size_t frameCount, const size_t frameSize,
        const size_t mdRange);
};

struct ComponentVibrationLocalizationTradition
    :ComponentVibrationLocalization {
    ReturnType MovingAverage(OTDRProcessValueType* pBuffer,
    const size_t frameCount, const size_t frameSize,
    const size_t maRange) override {
        const auto count = Util_MovingAverage(pBuffer,
            frameCount, frameSize,
            maRange);

        return std::make_tuple(pBuffer, count);
    }
    ReturnType MovingDifference(OTDRProcessValueType* pBuffer,
        const size_t frameCount, const size_t frameSize,
        const size_t mdRange) override {
        const auto count = Util_MovingDifference(pBuffer,
            frameCount, frameSize,
            mdRange);

        return std::make_tuple(pBuffer, count);
    }
};

