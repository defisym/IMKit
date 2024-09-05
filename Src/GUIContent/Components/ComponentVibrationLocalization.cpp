#include "ComponentVibrationLocalization.h"

ComponentVibrationLocalization::ComponentVibrationLocalization(const VibrationLocalizationParam& param) {
    this->m_param = param;
}

ComponentVibrationLocalizationTradition::ComponentVibrationLocalizationTradition(
    const VibrationLocalizationParam& param):ComponentVibrationLocalization(param) {
    bFilled = true;
}

ComponentVibrationLocalization::ReturnType ComponentVibrationLocalizationTradition::MovingAverage() {
    m_param.frameCount = Util_MovingAverage(m_param.pBuffer,
                                            m_param.frameCount, m_param.frameSize,
                                            m_param.maRange);

    return { m_param.pBuffer, m_param.frameCount };
}

ComponentVibrationLocalization::ReturnType ComponentVibrationLocalizationTradition::MovingDifference() {
    m_param.frameCount = Util_MovingDifference(m_param.pBuffer,
                                               m_param.frameCount, m_param.frameSize,
                                               m_param.mdRange);

    return { m_param.pBuffer, m_param.frameCount };
}

ComponentVibrationLocalizationContext::ComponentVibrationLocalizationContext(const VibrationLocalizationParam& param,
    VibrationLocalizationContextHandle hVibrationLocalization):ComponentVibrationLocalization(param),
                                                               m_hVibrationLocalization(hVibrationLocalization) {
    bFilled = Util_VibrationLocalizationContext_AddFrame(m_hVibrationLocalization, m_param.pBuffer);
}

ComponentVibrationLocalization::ReturnType ComponentVibrationLocalizationContext::MovingAverage() {
    if (!bFilled) { return { nullptr,0 }; }

    const auto pBuffer = Util_VibrationLocalizationContext_GetMovingAverageBuffer(m_hVibrationLocalization);
    const auto frameCount = Util_VibrationLocalizationContext_GetMovingAverageBufferFrameCount(m_hVibrationLocalization);

    return { pBuffer,frameCount };
}

ComponentVibrationLocalization::ReturnType ComponentVibrationLocalizationContext::MovingDifference() {
    if (!bFilled) { return { nullptr,0 }; }

    const auto pBuffer = Util_VibrationLocalizationContext_GetMovingDifferenceBuffer(m_hVibrationLocalization);
    const auto frameCount = Util_VibrationLocalizationContext_GetMovingDifferenceBufferFrameCount(m_hVibrationLocalization);

    return { pBuffer,frameCount };
}
