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

    return { m_param.frameCount };
}

const OTDRProcessValueType* ComponentVibrationLocalizationTradition::GetMovingAverageFrame(const size_t index) {
    return Context_GetConstFrameBuffer(m_param.pBuffer, m_param.frameSize, index);
}

ComponentVibrationLocalization::ReturnType ComponentVibrationLocalizationTradition::MovingDifference() {
    m_param.frameCount = Util_MovingDifference(m_param.pBuffer,
                                               m_param.frameCount, m_param.frameSize,
                                               m_param.mdRange);

    return { m_param.frameCount };
}

const OTDRProcessValueType* ComponentVibrationLocalizationTradition::GetMovingDifferenceFrame(const size_t index) {
   return Context_GetConstFrameBuffer(m_param.pBuffer, m_param.frameSize, index);
}

ComponentVibrationLocalizationContext::ComponentVibrationLocalizationContext(const VibrationLocalizationParam& param,
     const VibrationLocalizationContextParam& contextParam)
    :ComponentVibrationLocalization(param),
    m_hVibrationLocalization(contextParam.m_hVibrationLocalization) {
    // not updated, don't need to add this frame
    bFilled = Util_VibrationLocalizationContext_Filled(m_hVibrationLocalization) == 0;
    if (!contextParam.bUpdated) { return; }

    auto pFrame = m_param.pBuffer;
    for (size_t frameIndex = 0; frameIndex < m_param.frameCount; frameIndex++) {
        bFilled
            = Util_VibrationLocalizationContext_AddFrame(m_hVibrationLocalization, pFrame) == 0;
        pFrame += m_param.frameSize;
    }
}

ComponentVibrationLocalization::ReturnType ComponentVibrationLocalizationContext::MovingAverage() {
    if (!bFilled) { return { 0 }; }

    const auto frameCount = Util_VibrationLocalizationContext_GetMovingAverageBufferFrameCount(m_hVibrationLocalization);

    return { frameCount };
}

const OTDRProcessValueType* ComponentVibrationLocalizationContext::GetMovingAverageFrame(const size_t index) {
    return Util_VibrationLocalizationContext_GetMovingAverageFrame(m_hVibrationLocalization, index);
}

ComponentVibrationLocalization::ReturnType ComponentVibrationLocalizationContext::MovingDifference() {
    if (!bFilled) { return { 0 }; }

    const auto frameCount = Util_VibrationLocalizationContext_GetMovingDifferenceBufferFrameCount(m_hVibrationLocalization);

    return { frameCount };
}

const OTDRProcessValueType* ComponentVibrationLocalizationContext::GetMovingDifferenceFrame(const size_t index) {
    return Util_VibrationLocalizationContext_GetMovingDifferenceFrame(m_hVibrationLocalization, index);
}
