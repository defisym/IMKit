#include "ComponentWavefromsProcess.h"
#include "ComponentVibrationLocalization.h"

#include "../../IMGuiEx/DisplayPlot.h"
#include "../../IMGuiEx/AddSpin.h"
#include "../../IMGuiEx/DisableHelper.h"

#include "../../Src/Utilities/Buffer.h"

inline auto CastBufferPointer(const ComponentWavefromsProcess::BufferHandle h) {
    return static_cast<IndexBuffer<>*>(h);
}

ComponentWavefromsProcess::ComponentWavefromsProcess(Ctx* p, const OTDRContextHandle h)
:ComponentBase(p), hContext(h) {
	const auto& deviceParams = pCtx->deviceParams;
	const auto& processParams = pCtx->processParams;

    // alloc buffer
    const auto bufferSz = deviceParams.processFrameCount* deviceParams.pointNumPerScan;

    pWaveBuffer = new IndexBuffer(bufferSz);
    pWaveDisplayBuffer = new IndexBuffer(bufferSz);

    if (!deviceParams.bUseCountext) { return; }
	Util_VibrationLocalizationContext_Create(deviceParams.processFrameCount, deviceParams.pointNumPerScan,
											 processParams.movingAvgRange, processParams.movingDiffRange);
}

ComponentWavefromsProcess::~ComponentWavefromsProcess() {
    delete CastBufferPointer(pWaveBuffer);
    delete CastBufferPointer(pWaveDisplayBuffer);
	Util_VibrationLocalizationContext_Delete(&hVibrationLocalization);
}

void ComponentWavefromsProcess::Raw() const {
	if (!ImGui::BeginTabItem("Raw")) { return; }

	const auto& [pBuffer,
		bufferSz,
		bufferStride,
		bufferFrameCount,
		bufferFrameSize] = pCtx->deviceHandler.bufferInfo;

	using DataType = std::remove_cvref_t<std::remove_pointer_t<decltype(pBuffer)>>;
	auto stride = static_cast<int>(sizeof(DataType) * bufferStride);

	if (ImPlot::BeginPlot("ImPlot/Raw/CH1", plotSize)) {
		DisplayPlot("ImPlot/Raw/CH1/Plot",
					pBuffer,
					static_cast<int>(bufferFrameSize),
					static_cast<int>(bufferStride));

		ImPlot::EndPlot();
	}
	if (ImPlot::BeginPlot("ImPlot/Raw/CH2", plotSize)) {
		DisplayPlot("ImPlot/Raw/CH2/Plot",
					pBuffer + 1,
					static_cast<int>(bufferFrameSize),
					static_cast<int>(bufferStride));

		ImPlot::EndPlot();
	}

	ImGui::EndTabItem();

}

void ComponentWavefromsProcess::Shake() const {
	if (!ImGui::BeginTabItem("Shake")) { return; }

	const auto& bufferInfo = pCtx->deviceHandler.bufferInfo;
	const auto& deviceParams = pCtx->deviceParams;
	const auto& processParams = pCtx->processParams;

    const VibrationLocalizationParam param = 
    { Context_GetProcessBuffer(hContext, 1),
        bufferInfo.frameCount, bufferInfo.frameSize,
        processParams.movingAvgRange, processParams.movingDiffRange };

    const auto pComponentVibrationLocalization
    = [&] ()->std::unique_ptr<ComponentVibrationLocalization> {        
        if (deviceParams.bUseCountext) {
            VibrationLocalizationContextParam contextParam
                = { pCtx->deviceHandler.bContextUpdated,hVibrationLocalization };
            return std::make_unique<ComponentVibrationLocalizationContext>(param, contextParam);
        }

        return std::make_unique<ComponentVibrationLocalizationTradition>(param);
        }();

    if(!pComponentVibrationLocalization->bFilled) {
        ImGui::TextUnformatted("Data not enough");

        return;
    }

	if (ImGui::BeginTabBar("Shake/Tab", tabBarFlags)) {
        // Handle Moving Average
        {
            auto [pResult, frameCount]
                = pComponentVibrationLocalization->MovingAverage();

            if (ImGui::BeginTabItem("Shake MA")) {
                if (ImPlot::BeginPlot("ImPlot/Shake/MA", plotSize)) {
                    for (size_t frameIdx = 0; frameIdx < frameCount; frameIdx++) {
                        DisplayPlot(std::format("ImPlot/Shake/MA/Plot_{}", frameIdx).c_str(),
                            Context_GetConstFrameBuffer(pResult, param.frameSize, frameIdx),
                            static_cast<int>(param.frameSize));
                    }

                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
        }

        // Handle Moving Difference
        {
            auto [pResult, frameCount]
                = pComponentVibrationLocalization->MovingDifference();
            const auto accumulateFrameIndex = frameCount - 1;

            if (ImGui::BeginTabItem("Shake MD")) {
                if (ImPlot::BeginPlot("ImPlot/Shake/MD", plotSize)) {
                    for (size_t frameIdx = 0; frameIdx < accumulateFrameIndex; frameIdx++) {
                        DisplayPlot(std::format("ImPlot/Shake/MD/Plot_{}", frameIdx).c_str(),
                            Context_GetConstFrameBuffer(pResult, param.frameSize, frameIdx),
                            static_cast<int>(param.frameSize));
                    }

                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shake MD Accumulate")) {
                if (ImPlot::BeginPlot("ImPlot/Shake/MD/Accumulate", plotSize)) {
                    DisplayPlot("ImPlot/Shake/MD/Accumulate/Plot",
                          Context_GetConstFrameBuffer(pResult, param.frameSize, accumulateFrameIndex),
                          static_cast<int>(param.frameSize));
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
        }
		ImGui::EndTabBar();
	}

	ImGui::EndTabItem();
}

void ComponentWavefromsProcess::Wave() {
    if (!ImGui::BeginTabItem("Wave")) { return; }

    const auto waveRestoreOpt = GetWaveRestoreOpt();
    const auto bFilled = WaveProcess(waveRestoreOpt);

    do {
        if (!bFilled) {
            ImGui::TextUnformatted("Data not enough");
            break;
        }

        WaveDisplay();
    } while (false);

    ImGui::EndTabBar();
}

ComponentWavefromsProcess::WaveRestoreOpt ComponentWavefromsProcess::GetWaveRestoreOpt() const {
    // ------------------------------------
    // Params
    // ------------------------------------
    const auto& bufferInfo = pCtx->deviceHandler.bufferInfo;

    // ------------------------------------
    // Reference
    // ------------------------------------
    static bool bUseReference = false;
    ImGui::Checkbox("use reference", &bUseReference);
    ImGui::SameLine();
    ImGui::TextUnformatted("reference shares the range & unwrap settings");

    auto disableReference = ManualDisableHelper();

    disableReference.Disable(!bUseReference);

    static int referenceStart = 50;
    ImGui::SliderInt("##reference start", &referenceStart,
                     0, static_cast<int>(bufferInfo.frameSize),
                     "%d", sliderFlags);
    AddSpin("reference start", &referenceStart,
            0, static_cast<int>(bufferInfo.frameSize));
    ImGui::SameLine();
    ImGui::TextUnformatted("reference start");

    disableReference.Enable();

    // ------------------------------------
    // Shake
    // ------------------------------------
    static bool diff = false;
    ImGui::Checkbox("use diff", &diff);

    // TODO find the possible shake start position
    static int shakeStart = 50;
    ImGui::SliderInt("##shake start", &shakeStart,
                     0, static_cast<int>(bufferInfo.frameSize),
                     "%d", sliderFlags);
    AddSpin("shake start", &shakeStart,
            0, static_cast<int>(bufferInfo.frameSize));
    ImGui::SameLine();
    ImGui::TextUnformatted("shake start");

    static int shakeRange = 20;
    ImGui::SliderInt("##shake range", &shakeRange,
                     0, static_cast<int>(bufferInfo.frameSize) - shakeStart,
                     "%d", sliderFlags);
    AddSpin("shake range", &shakeRange,
            0, static_cast<int>(bufferInfo.frameSize) - shakeStart);
    ImGui::SameLine();
    ImGui::TextUnformatted("shake range");

    // ------------------------------------
    // Unwarp 2D
    // ------------------------------------
    static int unwrap2DStart = 1;
    ImGui::SliderInt("##unwrap 2D start", &unwrap2DStart,
                     1, shakeRange,
                     "%d", sliderFlags);
    AddSpin("unwrap 2D start", &unwrap2DStart,
            1, shakeRange);
    ImGui::SameLine();
    ImGui::TextUnformatted("unwrap 2D start");

    // ------------------------------------
    // Audio
    // ------------------------------------
    bool bPlayAudio = false;
    ImGui::Checkbox("ImPlot/Wave/Play Wave", &bPlayAudio);

    // ------------------------------------
    // Return
    // ------------------------------------
    return { {diff,shakeStart,shakeRange,unwrap2DStart},
        bUseReference,referenceStart,bPlayAudio };
}

bool ComponentWavefromsProcess::WaveProcess(const WaveRestoreOpt& opt) {
    // ------------------------
    // Basic info
    // ------------------------

    const auto& bufferInfo = pCtx->deviceHandler.bufferInfo;

    const auto bContextUpdated = pCtx->deviceHandler.bContextUpdated;
    const auto pIndexWaveBuffer = CastBufferPointer(pWaveBuffer);
    const auto pIndexWaveDisplayBuffer = CastBufferPointer(pWaveDisplayBuffer);

    // ------------------------
    // Update & Process buffer
    // ------------------------

    // always update buffer in both modes
    do {
        if (!bContextUpdated) { break; }

        const auto pProcess = Context_GetProcessBuffer(hContext, 0);

        if (pIndexWaveBuffer->Filled()) { pIndexWaveBuffer->Reset(); }

        const auto pCur = pIndexWaveBuffer->GetData();
        const auto dataSz = bufferInfo.frameCount * bufferInfo.frameSize;
        pIndexWaveBuffer->AddData(pProcess, dataSz);

        OTDRProcessValueType offset = 0.0f;
        OTDRProcessValueType scale = 0.0f;
        Util_WaveNormalization(pCur, dataSz, &offset, &scale);
        for (size_t idx = 0; idx < dataSz; idx++) { pCur[idx] *= PI; }

        if (!pIndexWaveBuffer->Filled()) { break; }

        pIndexWaveDisplayBuffer->Copy(pIndexWaveBuffer);
        WaveRestore(pIndexWaveBuffer->_pBuf, opt);
    } while (false);

    return pIndexWaveBuffer->Filled();  
}

void ComponentWavefromsProcess::WaveRestore(OTDRProcessValueType* pProcess, const WaveRestoreOpt& opt) {
    // ------------------------------------
    // Wave Restore
    // ------------------------------------
    this->WaveRestoreProcess(pProcess,
        { opt.diff,opt.shakeStart,opt.shakeRange,opt.unwrap2DStart },
        restoreWaveBuffer);

    // remove system noise by reference point
    if (opt.bUseReference) {
        this->WaveRestoreProcess(pProcess,
            { opt.diff,opt.referenceStart,opt.shakeRange,opt.unwrap2DStart },
            referenceWaveBuffer);

        for (size_t index = 0; index < referenceWaveBuffer.size(); index++) {
            restoreWaveBuffer[index] -= referenceWaveBuffer[index];
        }
    }

    // ------------------------------------
    // FFT
    // ------------------------------------
    restoreWaveFFTBuffer = restoreWaveBuffer;
    [[maybe_unused]] const auto fftElement = Util_FFT_Amplitude(restoreWaveFFTBuffer.data(), restoreWaveFFTBuffer.size());

    // ------------------------------------
    // Handle Audio Data
    // ------------------------------------
    if (!opt.bPlayAudio) { return; }
    memcpy(pCtx->audioHandler.GetBuffer(),
        restoreWaveBuffer.data(),
        sizeof(OTDRProcessValueType) * restoreWaveBuffer.size());
}

void ComponentWavefromsProcess::WaveRestoreProcess(OTDRProcessValueType* pProcess, const ShakeInfo& shakeInfo,
                                                   std::vector<OTDRProcessValueType>& waveBuffer) const {
	const auto& bufferInfo = pCtx->deviceHandler.bufferInfo;
	const auto& [diff, 
		shakeStart, 
		shakeRange,
		unwrap2DStart] = shakeInfo;

	Util_IterateFrames(pProcess, bufferInfo.frameCount, bufferInfo.frameSize,
					   [] (OTDRProcessValueType* pFrame, const size_t frameSz, void* pUserData) {
						   const auto pShakeInfo = static_cast<ShakeInfo* const>(pUserData);
						   const auto pStart = pFrame + pShakeInfo->shakeStart;

						   Util_EliminateInitialInterference(pStart, pShakeInfo->shakeRange);
						   Util_Unwrap(pStart, pShakeInfo->shakeRange, PI);
						   if (!pShakeInfo->diff) { return; }
						   Util_Diff(pStart, pShakeInfo->shakeRange);
					   }, const_cast<ShakeInfo*>(&shakeInfo));
				
	waveBuffer.resize(bufferInfo.frameCount);

	const auto pStart = pProcess + shakeStart + unwrap2DStart;
	for (size_t index = 0; index < bufferInfo.frameCount; index++) {
		waveBuffer[index] = pStart[index * bufferInfo.frameSize];
	}

	Util_Unwrap(waveBuffer.data(), waveBuffer.size(), PI);
}

void ComponentWavefromsProcess::WaveDisplay() const {
    if (!ImGui::BeginTabBar("Wave/Tab", tabBarFlags)) { return; }

    const auto& deviceParams = pCtx->deviceParams;

    if (ImGui::BeginTabItem("Wave Unprocessed")) {
        if (ImPlot::BeginPlot("ImPlot/Wave/Wave Unprocessed", plotSize)) {
            for (size_t frameIdx = 0;
                frameIdx < static_cast<size_t>(deviceParams.processFrameCount);
                frameIdx++) {
                DisplayPlot(std::format("ImPlot/Wave/Wave Unprocessed/Plot_{}", frameIdx).c_str(),
                    Context_GetFrameBuffer(CastBufferPointer(pWaveDisplayBuffer)->_pBuf,
                    deviceParams.pointNumPerScan, frameIdx),
                    deviceParams.pointNumPerScan);
            }

            ImPlot::EndPlot();
        }
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Wave Shake")) {
        if (ImPlot::BeginPlot("ImPlot/Wave/Wave Shake", plotSize)) {
            DisplayPlot(std::format("ImPlot/Wave/Wave Shake").c_str(),
                restoreWaveBuffer.data(), static_cast<int>(restoreWaveBuffer.size()));

            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("ImPlot/Wave/Wave FFT Amplitude", plotSize)) {
            DisplayPlot(std::format("ImPlot/Wave/Wave FFT Amplitude").c_str(),
                restoreWaveFFTBuffer.data(), static_cast<int>(restoreWaveFFTBuffer.size()), 1,
                [&] (const double index) {
                        return static_cast<double>(Util_FFT_GetFrequency(static_cast<size_t>(index),
                            restoreWaveFFTBuffer.size(), static_cast<float>(deviceParams.scanRate)));
                });

            ImPlot::EndPlot();
        }

        ImGui::EndTabItem();
    }

    ImGui::EndTabItem();
}
