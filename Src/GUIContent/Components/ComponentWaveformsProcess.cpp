#include "ComponentWaveformsProcess.h"
#include "ComponentVibrationLocalization.h"

#include <format>

#include "../../IMGuiEx/DisplayPlot.h"
#include "../../IMGuiEx/AddSpin.h"
#include "../../IMGuiEx/DisableHelper.h"
#include "../../IMGuiEx/EmbraceHelper.h"

#include "../../Src/Utilities/Buffer.h"

inline auto CastBufferPointer(const ComponentWaveformsProcess::BufferHandle h) {
    return static_cast<IndexBuffer<>*>(h);
}

ComponentWaveformsProcess::ComponentWaveformsProcess(Ctx* p)
    :ComponentBase(p), hContext(p->deviceHandler.hContext) {
	const auto& deviceParams = pCtx->deviceParams;

    // alloc buffer
    const auto bufferSz = deviceParams.processFrameCount * deviceParams.pointNumPerScan;

    pWaveBuffer = new IndexBuffer(bufferSz);
    pWaveDisplayBuffer = new IndexBuffer(bufferSz);

    hHighPassFilter = Util_Filter_CreateHighPassFilter(DEFAULT_ORDER,
        pCtx->deviceParams.scanRate,
        DEFAULT_CUTOFF_FREQUENCY);
    hMeanFilter = Util_Filter_CreateMeanFilter(DEFAULT_MEAN_RADIUS);
}

ComponentWaveformsProcess::~ComponentWaveformsProcess() {
    delete CastBufferPointer(pWaveBuffer);
    delete CastBufferPointer(pWaveDisplayBuffer);

    Util_Filter_DeleteFilter(&hHighPassFilter);
    Util_Filter_DeleteFilter(&hMeanFilter);
}

size_t ComponentWaveformsProcess::GetDisplayFrame(const size_t frameCount) {
    constexpr size_t MAX_DISPLAY_FRAME = 15;
    return std::min(frameCount, MAX_DISPLAY_FRAME);
}

void ComponentWaveformsProcess::WaveformTab() {
    if (!ImGui::BeginTabBar("Waveforms/Tab", TAB_BAR_FLAGS)) { return; }

    this->Raw();
    this->Shake();
    this->Wave();

    ImGui::EndTabBar();
}

void ComponentWaveformsProcess::Raw() const {
	if (!ImGui::BeginTabItem("Raw")) { return; }

	const auto& [pBuffer,
		bufferSz,
		bufferStride,
		bufferFrameCount,
		bufferFrameSize] = pCtx->deviceHandler.bufferInfo;

	using DataType = std::remove_cvref_t<std::remove_pointer_t<decltype(pBuffer)>>;
    [[maybe_unused]] auto stride = static_cast<int>(sizeof(DataType) * bufferStride);

	if (ImPlot::BeginPlot("ImPlot/Raw/CH1", PLOT_SIZE)) {
		DisplayPlot("ImPlot/Raw/CH1/Plot",
					pBuffer,
					static_cast<int>(bufferFrameSize),
					static_cast<int>(bufferStride));

		ImPlot::EndPlot();
	}
	if (ImPlot::BeginPlot("ImPlot/Raw/CH2", PLOT_SIZE)) {
		DisplayPlot("ImPlot/Raw/CH2/Plot",
					pBuffer + 1,
					static_cast<int>(bufferFrameSize),
					static_cast<int>(bufferStride));

		ImPlot::EndPlot();
	}

	ImGui::EndTabItem();
}

void ComponentWaveformsProcess::Shake() const {
    const EmbraceHelper tabHelper = { ImGui::BeginTabItem("Shake"), ImGui::EndTabItem };
	if (!tabHelper.State()) { return; }

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
                = { pCtx->deviceHandler.bContextUpdated, pCtx->processHandler.hVibrationLocalization };
            return std::make_unique<ComponentVibrationLocalizationContext>(param, contextParam);
        }

        return std::make_unique<ComponentVibrationLocalizationTradition>(param);
        }();

    if(!pComponentVibrationLocalization->bFilled) {
        ImGui::TextUnformatted("Data not enough");

        return;
    }

	if (ImGui::BeginTabBar("Shake/Tab", TAB_BAR_FLAGS)) {
        // Handle Moving Average
        {
            auto frameCount = pComponentVibrationLocalization->MovingAverage();

            if (ImGui::BeginTabItem("Shake MA")) {
                if (ImPlot::BeginPlot("ImPlot/Shake/MA", PLOT_SIZE)) {                    
                    for (size_t frameIdx = 0; frameIdx < GetDisplayFrame(frameCount); frameIdx++) {
                        DisplayPlot(std::format("ImPlot/Shake/MA/Plot_{}", frameIdx).c_str(),
                            pComponentVibrationLocalization->GetMovingAverageFrame(frameIdx),
                            static_cast<int>(param.frameSize));
                    }

                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
        }

        // Handle Moving Difference
        {
            auto frameCount = pComponentVibrationLocalization->MovingDifference();
            const auto accumulateFrameIndex = frameCount - 1;

            if (ImGui::BeginTabItem("Shake MD")) {
                if (ImPlot::BeginPlot("ImPlot/Shake/MD", PLOT_SIZE)) {
                    for (size_t frameIdx = 0; frameIdx < GetDisplayFrame(accumulateFrameIndex); frameIdx++) {
                        DisplayPlot(std::format("ImPlot/Shake/MD/Plot_{}", frameIdx).c_str(),
                            pComponentVibrationLocalization->GetMovingDifferenceFrame(frameIdx),
                            static_cast<int>(param.frameSize));
                    }

                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shake MD Accumulate")) {
                if (ImPlot::BeginPlot("ImPlot/Shake/MD/Accumulate", PLOT_SIZE)) {
                    DisplayPlot("ImPlot/Shake/MD/Accumulate/Plot",
                          pComponentVibrationLocalization->GetMovingDifferenceFrame(accumulateFrameIndex),
                          static_cast<int>(param.frameSize));
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
        }
		ImGui::EndTabBar();
	}
}

void ComponentWaveformsProcess::Wave() {
    if (!ImGui::BeginTabItem("Wave")) { return; }

    const auto waveRestoreOpt = GetWaveRestoreOpt();
    const auto bDisplayBufferFilled = WaveProcess(waveRestoreOpt);

    do {
        if (!bDisplayBufferFilled) {
            ImGui::TextUnformatted("Data not enough");
            break;
        }

        WaveDisplay();
    } while (false);

    ImGui::EndTabItem();
}

ComponentWaveformsProcess::WaveRestoreOpt ComponentWaveformsProcess::GetWaveRestoreOpt() {
    // ------------------------------------
    // Params
    // ------------------------------------
    bOptChanged = false;
    const auto& deviceParams = pCtx->deviceParams;
    const auto& bufferInfo = pCtx->deviceHandler.bufferInfo;

    // ------------------------------------
    // Reference
    // ------------------------------------
    ImGui::TextUnformatted("reference shares the range & unwrap settings");
    static bool bUseReference = false;
    bOptChanged &= ImGui::Checkbox("use reference", &bUseReference);
    ImGui::SameLine();
    static bool bReferenceAverage = false;
    bOptChanged &= ImGui::Checkbox("reference average", &bReferenceAverage);

    auto disableReference = ManualDisableHelper();

    disableReference.Disable(!bUseReference);

    static int referenceStart = 50;
    bOptChanged &= ImGui::SliderInt("##reference start", &referenceStart,
                     0, static_cast<int>(bufferInfo.frameSize),
                     "%d", SLIDER_FLAGS);
    bOptChanged &= AddSpin("reference start", &referenceStart,
            0, static_cast<int>(bufferInfo.frameSize));
    ImGui::SameLine();
    ImGui::TextUnformatted("reference start");

    disableReference.Enable();

    // ------------------------------------
    // Shake
    // ------------------------------------
    static bool diff = false;
    bOptChanged &= ImGui::Checkbox("use diff", &diff);

    // TODO find the possible shake start position
    static int shakeStart = 50;
    bOptChanged &= ImGui::SliderInt("##shake start", &shakeStart,
                     0, static_cast<int>(bufferInfo.frameSize),
                     "%d", SLIDER_FLAGS);
    bOptChanged &= AddSpin("shake start", &shakeStart,
            0, static_cast<int>(bufferInfo.frameSize));
    ImGui::SameLine();
    ImGui::TextUnformatted("shake start");

    static int shakeRange = 20;
    bOptChanged &= ImGui::SliderInt("##shake range", &shakeRange,
                     0, static_cast<int>(bufferInfo.frameSize) - shakeStart,
                     "%d", SLIDER_FLAGS);
    bOptChanged &= AddSpin("shake range", &shakeRange,
            0, static_cast<int>(bufferInfo.frameSize) - shakeStart);
    ImGui::SameLine();
    ImGui::TextUnformatted("shake range");

    // ------------------------------------
    // Unwarp 2D
    // ------------------------------------
    static int unwrap2DStart = 1;
    bOptChanged &= ImGui::SliderInt("##unwrap 2D start", &unwrap2DStart,
                     1, shakeRange,
                     "%d", SLIDER_FLAGS);
    bOptChanged &= AddSpin("unwrap 2D start", &unwrap2DStart,
            1, shakeRange);
    ImGui::SameLine();
    ImGui::TextUnformatted("unwrap 2D start");

    static int averageRange = 1;
    bOptChanged &= ImGui::SliderInt("##average range", &averageRange,
                     1, shakeRange - unwrap2DStart,
                     "%d", SLIDER_FLAGS);
    bOptChanged &= AddSpin("average range", &averageRange,
            1, shakeRange - unwrap2DStart);
    ImGui::SameLine();
    ImGui::TextUnformatted("average range");

    // ------------------------------------
    // Filter
    // ------------------------------------
    auto disableFilter = ManualDisableHelper();

    // ------------------------
    // High Pass Filter
    // ------------------------
    static bool bHighPassFilter = false;
    bOptChanged &= ImGui::Checkbox("High Pass Filter", &bHighPassFilter);
    ImGui::SameLine();

    disableFilter.Disable(!bHighPassFilter);

    static int filterStopFrequency = 20;
    static bool bHighPassFilterChanged = false;
    bHighPassFilterChanged &= ImGui::SliderInt("##Filter Stop Frequency", &filterStopFrequency,
        1, static_cast<int>(deviceParams.scanRate),
        "%d", SLIDER_FLAGS);
    bHighPassFilterChanged &= AddSpin("Filter Stop Frequency", &filterStopFrequency,
        1, static_cast<int>(deviceParams.scanRate));
    bOptChanged &= bHighPassFilterChanged;
    ImGui::SameLine();
    ImGui::TextUnformatted("Filter Stop Frequency");

    disableFilter.Enable();

    // ------------------------
    // Mean Filter
    // ------------------------
    static bool bMeanFilter = false;
    bOptChanged &= ImGui::Checkbox("Mean Filter", &bMeanFilter);
    ImGui::SameLine();

    disableFilter.Disable(!bMeanFilter);

    static int filterMeanRadius = 5;
    static bool bMeanFilterChanged = false;
    bMeanFilterChanged &= ImGui::SliderInt("##Filter Mean Radius", &filterMeanRadius,
        1, deviceParams.processFrameCount,
        "%d", SLIDER_FLAGS);
    bMeanFilterChanged &= AddSpin("Filter Mean Radius", &filterMeanRadius,
        1, deviceParams.processFrameCount);
    bOptChanged &= bMeanFilterChanged;
    ImGui::SameLine();
    ImGui::TextUnformatted("Filter Mean Radius");

    disableFilter.Enable();

    // ------------------------------------
    // Audio
    // ------------------------------------
    static bool bPlayAudio = false;
    bOptChanged &= ImGui::Checkbox("Play Wave", &bPlayAudio);

    // ------------------------------------
    // Return
    // ------------------------------------
    return { {diff,
        shakeStart,shakeRange,
        unwrap2DStart,averageRange},
        bPlayAudio,
        bUseReference,bReferenceAverage,referenceStart,
        { { bHighPassFilter,bHighPassFilterChanged },filterStopFrequency },
        { { bMeanFilter,bMeanFilterChanged }, filterMeanRadius }
    };
}

bool ComponentWaveformsProcess::WaveProcess(const WaveRestoreOpt& opt) {
    // ------------------------
    // Basic info
    // ------------------------

    auto& audioHandler = pCtx->audioHandler;

    const auto& deviceParams = pCtx->deviceParams;
    const auto& bufferInfo = pCtx->deviceHandler.bufferInfo;

    const auto bContextUpdated = pCtx->deviceHandler.bContextUpdated;
    const auto pIndexWaveBuffer = CastBufferPointer(pWaveBuffer);
    const auto pIndexWaveDisplayBuffer = CastBufferPointer(pWaveDisplayBuffer);

    // ------------------------
    // Update & Process buffer
    // ------------------------

    // reset audio buffer
    if (!opt.bPlayAudio) { audioHandler.ResetBuffer(); }
    if (bOptChanged) { bOptChanged = false; audioHandler.ResetIndex(); }

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

        if (!opt.bPlayAudio) { break; }
        audioBuffer = restoreWaveBuffer;
        // Normalization: audio accepts data [ -1.0 ~ 1.0 ]
        Util_MeanNormalization(audioBuffer.data(), audioBuffer.size(), &offset, &scale);
        audioHandler.AddData({ audioBuffer.data(), audioBuffer.size(),
            static_cast<size_t>(1000.0 * deviceParams.processFrameCount / deviceParams.scanRate) });
    } while (false);

    // should return the fill state of display buffer
    // the buffer used to receive data's state will change periodically
    // and cause jitter
    return pIndexWaveDisplayBuffer->Filled();
}

void ComponentWaveformsProcess::WaveRestore(OTDRProcessValueType* pProcess, const WaveRestoreOpt& opt) {
    // ------------------------------------
    // Wave Restore
    // ------------------------------------
    auto shakeInfo = static_cast<ShakeInfo>(opt);

    this->WaveRestoreProcess(pProcess, shakeInfo, restoreWaveBuffer);

    // remove system noise by reference point
    if (opt.bUseReference) {
        shakeInfo.shakeStart = opt.referenceStart;
        this->WaveRestoreProcess(pProcess, shakeInfo, referenceWaveBuffer);

        if (opt.bReferenceAverage) {
            OTDRProcessValueType accumulate = 0.0f;
            for (const float& element : referenceWaveBuffer) {
                accumulate += element;
            }

            const OTDRProcessValueType average = accumulate / static_cast<OTDRProcessValueType>(referenceWaveBuffer.size());
            for (float& element : restoreWaveBuffer) {
                element -= average;
            }
        }
        else {
            for (size_t index = 0; index < restoreWaveBuffer.size(); index++) {
                restoreWaveBuffer[index] -= referenceWaveBuffer[index];
            }
        }
    }

    // ------------------------------------
    // Filter
    // ------------------------------------
    //TODO White noise should be removed
    if (opt.highPassFilterParam.bEnable) {
        if (opt.highPassFilterParam.bUpdate) {
            Util_Filter_DeleteFilter(&hHighPassFilter);
            hHighPassFilter = Util_Filter_CreateHighPassFilter(5, pCtx->deviceParams.scanRate, opt.highPassFilterParam.filterStopFrequency);
        }

        Util_Filter_Filter(hHighPassFilter, restoreWaveBuffer.data(), restoreWaveBuffer.size());
    }

    if (opt.meanFilterParam.bEnable) {
        if (opt.meanFilterParam.bUpdate) {
            Util_Filter_DeleteFilter(&hMeanFilter);
            hMeanFilter = Util_Filter_CreateMeanFilter(opt.meanFilterParam.radius);
        }

        Util_Filter_Filter(hMeanFilter, restoreWaveBuffer.data(), restoreWaveBuffer.size());
    }

    // ------------------------------------
    // FFT
    // ------------------------------------
    restoreWaveFFTBuffer = restoreWaveBuffer;
    fftElement = Util_FFT_Amplitude(restoreWaveFFTBuffer.data(), restoreWaveFFTBuffer.size());
}

void ComponentWaveformsProcess::WaveRestoreProcess(OTDRProcessValueType* pProcess, const ShakeInfo& shakeInfo,
                                                   std::vector<OTDRProcessValueType>& waveBuffer) const {
	const auto& deviceParams = pCtx->deviceParams;
    const auto& [diff,
        shakeStart, shakeRange,
        unwrap2DStart, averageRange] = shakeInfo;

	Util_IterateFrames(pProcess, deviceParams.processFrameCount, deviceParams.pointNumPerScan,
					   [] (OTDRProcessValueType* pFrame, const size_t frameSz, void* pUserData) {
						   const auto pShakeInfo = static_cast<ShakeInfo* const>(pUserData);
						   const auto pStart = pFrame + pShakeInfo->shakeStart;

						   Util_EliminateInitialInterference(pStart, pShakeInfo->shakeRange);
						   Util_Unwrap(pStart, pShakeInfo->shakeRange, PI);
						   if (!pShakeInfo->diff) { return; }
						   Util_Diff(pStart, pShakeInfo->shakeRange);
					   }, const_cast<ShakeInfo*>(&shakeInfo));
				
	waveBuffer.resize(deviceParams.processFrameCount);
    for (auto& it : waveBuffer) { it = 0.0f; }

    for (size_t averageIndex = 0; averageIndex < averageRange; averageIndex++) {
        const auto pStart = pProcess + shakeStart + unwrap2DStart + averageIndex;
        for (size_t index = 0; index < static_cast<size_t>(deviceParams.processFrameCount); index++) {
            waveBuffer[index] += pStart[index * deviceParams.pointNumPerScan];
        }
    }

    for (auto& it : waveBuffer) { it /= averageRange; }

	Util_Unwrap(waveBuffer.data(), waveBuffer.size(), PI);
}

void ComponentWaveformsProcess::WaveDisplay() const {
    if (!ImGui::BeginTabBar("Wave/Tab", TAB_BAR_FLAGS)) { return; }

    const auto& deviceParams = pCtx->deviceParams;

    if (ImGui::BeginTabItem("Wave Unprocessed")) {
        if (ImPlot::BeginPlot("ImPlot/Wave/Wave Unprocessed", PLOT_SIZE)) {
            for (size_t frameIdx = 0;
                frameIdx < GetDisplayFrame(deviceParams.processFrameCount);
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
        if (ImPlot::BeginPlot("ImPlot/Wave/Wave Shake", PLOT_SIZE)) {
            DisplayPlot(std::format("ImPlot/Wave/Wave Shake").c_str(),
                restoreWaveBuffer.data(), static_cast<int>(restoreWaveBuffer.size()));

            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("ImPlot/Wave/Wave FFT Amplitude", PLOT_SIZE)) {
            DisplayPlot(std::format("ImPlot/Wave/Wave FFT Amplitude").c_str(),
                restoreWaveFFTBuffer.data(), static_cast<int>(fftElement), 1,
                [&] (const double index) {
                    // use the original element size for frequency calculation
                    return static_cast<double>(Util_FFT_GetFrequency(static_cast<size_t>(index),
                        restoreWaveFFTBuffer.size(),
                        static_cast<float>(deviceParams.scanRate)));
                });

            ImPlot::EndPlot();
        }

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
}
