#include "ComponentWaveformProcess.h"

#include <format>

#include "../../IMGuiEx/DisplayPlot.h"
#include "../../IMGuiEx/EmbraceHelper.h"

#include "../../Src/Utilities/Buffer.h"

ComponentWaveformProcess::ComponentWaveformProcess(Ctx* p)
    :ComponentBase(p) {}

size_t ComponentWaveformProcess::GetDisplayFrame(const size_t frameCount) {
    constexpr size_t MAX_DISPLAY_FRAME = 15;
    return std::min(frameCount, MAX_DISPLAY_FRAME);
}

void ComponentWaveformProcess::WaveformTab() const {
    if (!ImGui::BeginTabBar("Waveform/Tab", TAB_BAR_FLAGS)) { return; }

    this->Raw();
    this->Shake();
    this->Wave();

    ImGui::EndTabBar();
}

void ComponentWaveformProcess::Raw() const {
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

void ComponentWaveformProcess::Shake() const {
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

void ComponentWaveformProcess::Wave() const {
    if (!ImGui::BeginTabItem("Wave")) { return; }

    auto pHandler = pCtx->processHandler.pWaveformRestoreHandler;
    const auto& waveRestoreOpt = pCtx->processHandler.processParams.wrParam;
    const auto bOptChanged = pCtx->processHandler.bWaveformRestoreOptUpdated;

    if (!pHandler->WaveProcess(waveRestoreOpt, bOptChanged)) {
        ImGui::TextUnformatted("Data not enough");
        return;
    }

    WaveDisplay();

    ImGui::EndTabItem();
}

void ComponentWaveformProcess::WaveDisplay() const {
    if (!ImGui::BeginTabBar("Wave/Tab", TAB_BAR_FLAGS)) { return; }

    const auto& deviceParams = pCtx->deviceHandler.deviceParams;
    const auto pHandler = pCtx->processHandler.pWaveformRestoreHandler;

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
