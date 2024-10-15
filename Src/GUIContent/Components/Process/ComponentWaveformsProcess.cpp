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

void ComponentWaveformsProcess::Wave() const {
    if (!ImGui::BeginTabItem("Wave")) { return; }

    auto pHandler = pCtx->processHandler.pWaveformsRestoreHandler;
    auto& waveRestoreOpt = pCtx->processHandler.processParams.wrParam;

    const auto bOptChanged = GetWaveRestoreOpt(waveRestoreOpt);
    const auto bDisplayBufferFilled = pHandler->WaveProcess(waveRestoreOpt, bOptChanged);

    if (!bDisplayBufferFilled) {
        ImGui::TextUnformatted("Data not enough");
        return;
    }

    WaveDisplay();

    ImGui::EndTabItem();
}

bool ComponentWaveformsProcess::GetWaveRestoreOpt(WaveformsRestoreHandler::WaveRestoreOpt& opt) const {
    // ------------------------------------
    // Params
    // ------------------------------------
    bool bOptChanged = false;
    const auto& deviceParams = pCtx->deviceHandler.deviceParams;
    const auto& bufferInfo = pCtx->deviceHandler.bufferInfo;

    // ------------------------------------
    // Reference
    // ------------------------------------
    ImGui::TextUnformatted("reference shares the range & unwrap settings");
    bOptChanged &= ImGui::Checkbox("use reference", &opt.bUseReference);
    ImGui::SameLine();

    auto disableReference = ManualDisableHelper();

    disableReference.Disable(!opt.bUseReference);

    bOptChanged &= ImGui::Checkbox("reference average", &opt.bReferenceAverage);
    bOptChanged &= ImGui::SliderInt("##reference start", &opt.referenceStart,
                     0, static_cast<int>(bufferInfo.frameSize),
                     "%d", SLIDER_FLAGS);
    bOptChanged &= AddSpin("reference start", &opt.referenceStart,
            0, static_cast<int>(bufferInfo.frameSize));
    ImGui::SameLine();
    ImGui::TextUnformatted("reference start");

    disableReference.Enable();

    // ------------------------------------
    // Shake
    // ------------------------------------
    bOptChanged &= ImGui::Checkbox("use diff", &opt.diff);

    // TODO find the possible shake start position
    bOptChanged &= ImGui::SliderInt("##shake start", &opt.shakeStart,
                     0, static_cast<int>(bufferInfo.frameSize),
                     "%d", SLIDER_FLAGS);
    bOptChanged &= AddSpin("shake start", &opt.shakeStart,
            0, static_cast<int>(bufferInfo.frameSize));
    ImGui::SameLine();
    ImGui::TextUnformatted("shake start");

    bOptChanged &= ImGui::SliderInt("##shake range", &opt.shakeRange,
                     0, static_cast<int>(bufferInfo.frameSize) - opt.shakeStart,
                     "%d", SLIDER_FLAGS);
    bOptChanged &= AddSpin("shake range", &opt.shakeRange,
            0, static_cast<int>(bufferInfo.frameSize) - opt.shakeStart);
    ImGui::SameLine();
    ImGui::TextUnformatted("shake range");

    // ------------------------------------
    // Unwrap 2D
    // ------------------------------------
    bOptChanged &= ImGui::SliderInt("##unwrap 2D start", &opt.unwrap2DStart,
                     1, opt.shakeRange,
                     "%d", SLIDER_FLAGS);
    bOptChanged &= AddSpin("unwrap 2D start", &opt.unwrap2DStart,
            1, opt.shakeRange);
    ImGui::SameLine();
    ImGui::TextUnformatted("unwrap 2D start");

    bOptChanged &= ImGui::SliderInt("##average range", &opt.averageRange,
                     1, opt.shakeRange - opt.unwrap2DStart,
                     "%d", SLIDER_FLAGS);
    bOptChanged &= AddSpin("average range", &opt.averageRange,
            1, opt.shakeRange - opt.unwrap2DStart);
    ImGui::SameLine();
    ImGui::TextUnformatted("average range");

    // ------------------------------------
    // Filter
    // ------------------------------------
    auto disableFilter = ManualDisableHelper();

    // ------------------------
    // High Pass Filter
    // ------------------------
    bOptChanged &= ImGui::Checkbox("High Pass Filter", &opt.highPassFilterParam.bEnable);
    ImGui::SameLine();

    disableFilter.Disable(!opt.highPassFilterParam.bEnable);

    opt.highPassFilterParam.bUpdate
        &= ImGui::SliderInt("##Filter Stop Frequency", &opt.highPassFilterParam.filterStopFrequency,
            1, static_cast<int>(deviceParams.scanRate),
            "%d", SLIDER_FLAGS);
    opt.highPassFilterParam.bUpdate
        &= AddSpin("Filter Stop Frequency", &opt.highPassFilterParam.filterStopFrequency,
            1, static_cast<int>(deviceParams.scanRate));
    bOptChanged &= opt.highPassFilterParam.bUpdate;
    ImGui::SameLine();
    ImGui::TextUnformatted("Filter Stop Frequency");

    disableFilter.Enable();

    // ------------------------
    // Mean Filter
    // ------------------------
    bOptChanged &= ImGui::Checkbox("Mean Filter", &opt.meanFilterParam.bEnable);
    ImGui::SameLine();

    disableFilter.Disable(!opt.meanFilterParam.bEnable);

    opt.meanFilterParam.bUpdate
        &= ImGui::SliderInt("##Filter Mean Radius", &opt.meanFilterParam.radius,
            1, deviceParams.processFrameCount,
            "%d", SLIDER_FLAGS);
    opt.meanFilterParam.bUpdate
        &= AddSpin("Filter Mean Radius", &opt.meanFilterParam.radius,
            1, deviceParams.processFrameCount);
    bOptChanged &= opt.meanFilterParam.bUpdate;
    ImGui::SameLine();
    ImGui::TextUnformatted("Filter Mean Radius");

    disableFilter.Enable();

    // ------------------------------------
    // Audio
    // ------------------------------------
    bOptChanged &= ImGui::Checkbox("Play Wave", &opt.bPlayAudio);

    // ------------------------------------
    // Return
    // ------------------------------------
    return bOptChanged;
}

void ComponentWaveformsProcess::WaveDisplay() const {
    if (!ImGui::BeginTabBar("Wave/Tab", TAB_BAR_FLAGS)) { return; }

    const auto& deviceParams = pCtx->deviceHandler.deviceParams;
    const auto pHandler = pCtx->processHandler.pWaveformsRestoreHandler;

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
