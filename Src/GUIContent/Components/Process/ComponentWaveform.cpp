#include "ComponentWaveform.h"

#include "macro.h"
#include "IMGuiEx/DisplayPlot.h"
#include "IMGuiEx/EmbraceHelper.h"

// do not display more than MAX_DISPLAY_FRAME
// for raw data -> nobody cares!
size_t GetDisplayFrame(const size_t frameCount) {
    constexpr size_t MAX_DISPLAY_FRAME = 15;
    return std::min(frameCount, MAX_DISPLAY_FRAME);
}

void RawData(Ctx* pCtx)  {
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

void VibrationLocalization(Ctx* pCtx)  {
    const EmbraceHelper tabHelper = { ImGui::BeginTabItem("Vibration Localization"), ImGui::EndTabItem };
    if (!tabHelper.State()) { return; }
    const auto frameSz = pCtx->deviceHandler.bufferInfo.frameSize;
    const auto frameSize = static_cast<int>(frameSz);
    const auto pComponentVibrationLocalization = pCtx->processHandler.GetVibrationLocalizationHandler(pCtx, Context_GetProcessBuffer(pCtx->deviceHandler.hContext, 1));

    if (!pComponentVibrationLocalization->bFilled) {
        ImGui::TextUnformatted("Data not enough");

        return;
    }

    // for logger
    const OTDRProcessValueType* pResult = nullptr;

#ifndef VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
    if (ImGui::BeginTabBar("Shake/Tab", TAB_BAR_FLAGS)) {
#endif
        // Handle Moving Average
        {
            auto frameCount = pComponentVibrationLocalization->MovingAverage();
#ifndef VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
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
#endif
        }

        // Handle Moving Difference
        {
            auto frameCount = pComponentVibrationLocalization->MovingDifference();
            const auto accumulateFrameIndex = frameCount - 1;
            pResult = pComponentVibrationLocalization->GetMovingDifferenceFrame(accumulateFrameIndex);
#ifndef VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
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
#endif
                if (ImPlot::BeginPlot("ImPlot/Shake/MD/Accumulate", PLOT_SIZE)) {
#ifdef VIBRATION_LOCALIZATION_SHOW_LOGGER_THRESHOLD
                    auto threshold = pCtx->loggerHandler.loggerParams.threshold;

                    ImPlot::PlotLineG("ImPlot/Shake/MD/Accumulate/Threshold",
                        [] (int idx, void* pData) {
                            return ImPlotPoint{ static_cast<double>(idx),
                                static_cast<double>(*static_cast<decltype(threshold)*>(pData)) };
                        }, & threshold, frameSize, ImPlotLineFlags_Shaded);
#endif
                    DisplayPlot("ImPlot/Shake/MD/Accumulate/Plot", pResult, frameSize);

                    ImPlot::EndPlot();
                }
#ifndef VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
                ImGui::EndTabItem();
            }
#endif
        }

#ifndef VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
        ImGui::EndTabBar();
    }
#endif

    auto& logger = pCtx->loggerHandler.vibrationLogger;
    logger.loggerData.UpdateData({ pResult, frameSz });
    logger.AddData();
}

void WaveformRestore(const Ctx* pCtx) {
    const EmbraceHelper tabItemHelper = { ImGui::BeginTabItem("Waveform Restore"), ImGui::EndTabItem };
    if (!tabItemHelper.State()) { return; }

    const auto pHandler = pCtx->processHandler.pWaveformRestoreHandler;
    const auto bOptChanged = pCtx->processHandler.bWaveformRestoreOptUpdated;
    const auto& waveRestoreOpt = pCtx->processHandler.processParams.wrParam;
    const auto& deviceParams = pCtx->deviceHandler.deviceParams;

    if (!pHandler->WaveProcess(waveRestoreOpt, bOptChanged)) {
        ImGui::TextUnformatted("Data not enough");
        return;
    }

#ifndef WAVEFORM_RESTORE_ONLY_SHOW_RESULT
    const EmbraceHelper tabBarHelper = { ImGui::BeginTabBar("Wave/Tab", TAB_BAR_FLAGS), ImGui::EndTabBar };
    if (!tabBarHelper.State()) { return; }

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
#endif
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
#ifndef WAVEFORM_RESTORE_ONLY_SHOW_RESULT
        ImGui::EndTabItem();
    }
#endif
}

// Note: ImGui call this each frame
//       if move to other UI lib should change the read logic
void ComponentWaveform(Ctx* pCtx) {
    // out of tab: should always pull data
    const auto err = pCtx->deviceHandler.ReadData();

	if (!ImGui::CollapsingHeader("Waveform", ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

    switch (err) {
    case DeviceHandler::ReadResult::NO_DEVICE:
        ImGui::TextUnformatted("Device not started");
        break;
	case DeviceHandler::ReadResult::NO_CONTEXT:
        ImGui::TextUnformatted("Context not created");
        break;
	case DeviceHandler::ReadResult::OK:
        if (ImGui::BeginTabBar("Waveform/Tab", TAB_BAR_FLAGS)) {
            RawData(pCtx);
            VibrationLocalization(pCtx);
            WaveformRestore(pCtx);

            ImGui::EndTabBar();
        }

        break;
    }
}
