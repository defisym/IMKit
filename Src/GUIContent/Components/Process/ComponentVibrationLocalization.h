// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
#pragma once

#include "../Basic//ComponentBase.h"

struct VibrationLocalizationParam {
    // info of read buffer
    OTDRProcessValueType* pBuffer = nullptr;
    size_t frameCount = 0;
    size_t frameSize = 0;
    // info of process
    size_t maRange = 0;
    size_t mdRange = 0;
};

struct VibrationLocalizationReturn {
    const OTDRProcessValueType* pBuffer = nullptr;
    size_t frameCount = 0;
};

struct ComponentVibrationLocalization {
    VibrationLocalizationParam m_param = {};
    bool bFilled = false;

    ComponentVibrationLocalization(const VibrationLocalizationParam& param);
    virtual ~ComponentVibrationLocalization() = default;

    using ReturnType = const size_t;

    virtual ReturnType MovingAverage() = 0;
    virtual const OTDRProcessValueType* GetMovingAverageFrame(const size_t index) = 0;
    virtual ReturnType MovingDifference() = 0;
    virtual const OTDRProcessValueType* GetMovingDifferenceFrame(const size_t index) = 0;
};

struct ComponentVibrationLocalizationTradition final
    :ComponentVibrationLocalization {
    ComponentVibrationLocalizationTradition(const VibrationLocalizationParam& param);

    ReturnType MovingAverage() override;
    const OTDRProcessValueType* GetMovingAverageFrame(const size_t index) override;
    ReturnType MovingDifference() override;
    const OTDRProcessValueType* GetMovingDifferenceFrame(const size_t index) override;
};

struct VibrationLocalizationContextParam {
    bool bUpdated = false;
    VibrationLocalizationContextHandle m_hVibrationLocalization = nullptr;
};

struct ComponentVibrationLocalizationContext final
    :ComponentVibrationLocalization {
    VibrationLocalizationContextHandle m_hVibrationLocalization = nullptr;

    ComponentVibrationLocalizationContext(const VibrationLocalizationParam& param,
        const VibrationLocalizationContextParam& contextParam);

    ReturnType MovingAverage() override;
    const OTDRProcessValueType* GetMovingAverageFrame(const size_t index) override;
    ReturnType MovingDifference() override;
    const OTDRProcessValueType* GetMovingDifferenceFrame(const size_t index) override;
};
