#include "ComponentWaveformsProcess.h"

#include <format>

#include "../../IMGuiEx/DisplayPlot.h"
#include "../../IMGuiEx/AddSpin.h"
#include "../../IMGuiEx/DisableHelper.h"
#include "../../IMGuiEx/EmbraceHelper.h"

#include "../../Src/Utilities/Buffer.h"

ComponentWaveformsProcess::ComponentWaveformsProcess(Ctx* p)
    :ComponentBase(p) {}

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
    const auto frameSize = static_cast<int>(pCtx->deviceHandler.bufferInfo.frameSize);
    const auto pComponentVibrationLocalization = pCtx->processHandler.GetVibrationLocalizationHandler(pCtx, Context_GetProcessBuffer(pCtx->deviceHandler.hContext, 1));

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
                            frameSize);
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
                           frameSize);
                    }

                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shake MD Accumulate")) {
                if (ImPlot::BeginPlot("ImPlot/Shake/MD/Accumulate", PLOT_SIZE)) {
                    DisplayPlot("ImPlot/Shake/MD/Accumulate/Plot",
                          pComponentVibrationLocalization->GetMovingDifferenceFrame(accumulateFrameIndex),
                         frameSize);
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
    const auto bDisplayBufferFilled = pCtx->processHandler.pWaveformsProcess->WaveProcess(waveRestoreOpt, bOptChanged);

    do {
        if (!bDisplayBufferFilled) {
            ImGui::TextUnformatted("Data not enough");
            break;
        }

        WaveDisplay();
    } while (false);

    ImGui::EndTabItem();
}

WaveformsRestoreHandler::WaveRestoreOpt ComponentWaveformsProcess::GetWaveRestoreOpt() {
    // ------------------------------------
    // Params
    // ------------------------------------
    bOptChanged = false;
    const auto& deviceParams = pCtx->deviceHandler.deviceParams;
    const auto& bufferInfo = pCtx->deviceHandler.bufferInfo;

    // ------------------------------------
    // Reference
    // ------------------------------------
    ImGui::TextUnformatted("reference shares the range & unwrap settings");
    static bool bUseReference = false;
    bOptChanged &= ImGui::Checkbox("use reference", &bUseReference);
    ImGui::SameLine();

    auto disableReference = ManualDisableHelper();

    disableReference.Disable(!bUseReference);

    static bool bReferenceAverage = false;
    bOptChanged &= ImGui::Checkbox("reference average", &bReferenceAverage);

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
    // Unwrap 2D
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

void ComponentWaveformsProcess::WaveDisplay() const {
    if (!ImGui::BeginTabBar("Wave/Tab", TAB_BAR_FLAGS)) { return; }

    const auto& deviceParams = pCtx->deviceHandler.deviceParams;
    const auto pHandler = pCtx->processHandler.pWaveformsProcess;

    if (ImGui::BeginTabItem("Wave Unprocessed")) {
        if (ImPlot::BeginPlot("ImPlot/Wave/Wave Unprocessed", PLOT_SIZE)) {
            for (size_t frameIdx = 0;
                frameIdx < GetDisplayFrame(deviceParams.processFrameCount);
                frameIdx++) {
                DisplayPlot(std::format("ImPlot/Wave/Wave Unprocessed/Plot_{}", frameIdx).c_str(),                    
                    Context_GetFrameBuffer(pHandler->pWaveDisplayBuffer->_pBuf,
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
                pHandler->restoreWaveBuffer.data(), 
                static_cast<int>(pHandler->restoreWaveBuffer.size()));

            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("ImPlot/Wave/Wave FFT Amplitude", PLOT_SIZE)) {
            DisplayPlot(std::format("ImPlot/Wave/Wave FFT Amplitude").c_str(),
                pHandler->restoreWaveFFTBuffer.data(),
                static_cast<int>(pHandler->fftElement), 1,
                [&] (const double index) {
                    // use the original element size for frequency calculation
                    return static_cast<double>(Util_FFT_GetFrequency(static_cast<size_t>(index),
                        pHandler->restoreWaveFFTBuffer.size(),
                        static_cast<float>(deviceParams.scanRate)));
                });

            ImPlot::EndPlot();
        }

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
}
