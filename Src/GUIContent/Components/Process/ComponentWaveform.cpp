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
    if (!ImGui::BeginTabItem(I18N("Raw"))) { return; }

    const auto& [pBuffer,
        bufferSz,
        bufferStride,
        bufferFrameCount,
        bufferFrameSize] = pCtx->deviceHandler.bufferInfo;

    using DataType = std::remove_cvref_t<std::remove_pointer_t<decltype(pBuffer)>>;
    [[maybe_unused]] auto stride = static_cast<int>(sizeof(DataType) * bufferStride);

    if (BeginPlotEx(I18NSTR("CH1", "ImPlot/Raw/CH1"), I18NSTR("Point"))) {
        DisplayPlot(I18N("CH1", "ImPlot/Raw/CH1/Plot"),
                    pBuffer,
                    static_cast<int>(bufferFrameSize),
                    static_cast<int>(bufferStride));

        ImPlot::EndPlot();
    }
    if (BeginPlotEx(I18NSTR("CH2", "ImPlot/Raw/CH2"), I18NSTR("Point"))) {
        DisplayPlot(I18N("CH2", "ImPlot/Raw/CH2/Plot"),
                    pBuffer + 1,
                    static_cast<int>(bufferFrameSize),
                    static_cast<int>(bufferStride));

        ImPlot::EndPlot();
    }

    ImGui::EndTabItem();
}

void VibrationLocalization(Ctx* pCtx) {
    const EmbraceHelper tabHelper = { ImGui::BeginTabItem(I18N("Vibration Localization")), ImGui::EndTabItem };
    if (!tabHelper.State()) { return; }
    const auto pHandler = pCtx->processHandler.pVibrationLocalizationHandler;
    const auto frameSz = pCtx->deviceHandler.bufferInfo.frameSize;
    const auto frameSize = static_cast<int>(frameSz);

#ifndef VIBRATION_LOCALIZATION_ALWAYS_UPDATE
    pCtx->processHandler.ProcessVibrationLocalization();
#endif

    if (!pCtx->processHandler.processResult.bVibrationLocalizationProcessed) {
        ImGui::TextUnformatted(I18N("Data not enough"));
        return;
    }

#ifndef VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
    if (ImGui::BeginTabBar("Shake/Tab", TAB_BAR_FLAGS)) {
        // Handle Moving Average
        if (ImGui::BeginTabItem(I18N("Shake MA"))) {
            if (BeginPlotEx(I18N("MA", "ImPlot/Shake/MA"))) {
                const auto maxFrameCount = GetDisplayFrame(pHandler->pProcessor->maFrameCount);
                for (size_t frameIdx = 0; frameIdx < maxFrameCount; frameIdx++) {
                    const std::string plotName = I18NFMT("Plot {}", frameIdx);
                    DisplayPlot(std::format("{}##ImPlot/Shake/MA/{}", plotName, plotName).c_str(),
                        pHandler->pProcessor->GetMovingAverageFrame(frameIdx),
                        frameSize);
                }

                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }

        // Handle Moving Difference
        if (ImGui::BeginTabItem(I18N("Shake MD"))) {
            if (BeginPlotEx(I18N("MD", "ImPlot/Shake/MD"))) {
                // accumulateFrameIndex
                const auto maxFrameCount = GetDisplayFrame(pHandler->pProcessor->mdFrameCount) - 1;

                for (size_t frameIdx = 0; frameIdx < maxFrameCount; frameIdx++) {
                    const std::string plotName = I18NFMT("Plot {}", frameIdx);
                    DisplayPlot(std::format("{}##ImPlot/Shake/MD/{}", plotName, plotName).c_str(),
                        pHandler->pProcessor->GetMovingDifferenceFrame(frameIdx),
                        frameSize);
                }

                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(I18N("Shake MD Accumulate"))) {
#endif                
            if (BeginPlotEx(I18N("Vibration Localization", "ImPlot/Shake/MD/Accumulate"))) {
#ifdef VIBRATION_LOCALIZATION_SHOW_LOGGER_THRESHOLD
                auto threshold = pCtx->loggerHandler.loggerParams.threshold;
                ImPlot::PlotLineG(I18N("Threshold", "ImPlot/Shake/MD/Accumulate/Threshold"),
                    [] (int idx, void* pData) {
                    return ImPlotPoint{ static_cast<double>(idx),
                        static_cast<double>(*static_cast<decltype(threshold)*>(pData)) };
                    }, &threshold, frameSize, ImPlotLineFlags_Shaded);
#endif
                DisplayPlot(I18N("Vibration Localization", "ImPlot/Shake/MD/Accumulate/Vibration Localization"),
                    pHandler->GetVibrationLocalizationResult(), frameSize);

                ImPlot::EndPlot();
            }
#ifndef VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
#endif

#ifndef VIBRATION_LOCALIZATION_ALWAYS_UPDATE
    pCtx->loggerHandler.LogVibration(pCtx);
#endif
}

void WaveformRestore(Ctx* pCtx) {
    const EmbraceHelper tabItemHelper = { ImGui::BeginTabItem(I18N("Waveform Restore")), ImGui::EndTabItem };
    if (!tabItemHelper.State()) { return; }

    const auto pHandler = pCtx->processHandler.pWaveformRestoreHandler;
    const auto& deviceParams = pCtx->deviceHandler.deviceParams;

#ifndef WAVEFORM_RESTORE_ALWAYS_UPDATE
    pCtx->processHandler.ProcessWaveform();
#endif

    if (!pCtx->processHandler.processResult.bWaveFromProcessed) {
        ImGui::TextUnformatted(I18N("Data not enough"));
        return;
    }

#ifndef WAVEFORM_RESTORE_ONLY_SHOW_RESULT
    const EmbraceHelper tabBarHelper = { ImGui::BeginTabBar("Wave/Tab", TAB_BAR_FLAGS), ImGui::EndTabBar };
    if (!tabBarHelper.State()) { return; }

    if (ImGui::BeginTabItem(I18N("Wave Unprocessed"))) {       
        if (BeginPlotEx(I18N("Wave Unprocessed", "ImPlot/Wave/Wave Unprocessed"))) {
            for (size_t frameIdx = 0;
                frameIdx < GetDisplayFrame(deviceParams.processFrameCount);
                frameIdx++) {
                const std::string plotName = I18NFMT("Plot {}", frameIdx);
                DisplayPlot(std::format("{}##ImPlot/Wave/Wave Unprocessed/{}", plotName, plotName).c_str(),
                    Context_GetFrameBuffer(pHandler->pWaveDisplayBuffer->_pBuf,
                    deviceParams.pointNumPerScan, frameIdx),
                    deviceParams.pointNumPerScan);
            }

            ImPlot::EndPlot();
        }
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem(I18N("Wave Shake"))) {
#endif
        if (BeginPlotEx(I18N("Wave Shake", "ImPlot/Wave/Wave Shake"))) {
            DisplayPlot(I18N("Wave Shake", "ImPlot/Wave/Wave Shake"),
                pHandler->restoreWaveBuffer.data(),
                static_cast<int>(pHandler->restoreWaveBuffer.size()));

            ImPlot::EndPlot();
        }

        if (BeginPlotEx(I18N("Wave FFT Amplitude", "ImPlot/Wave/Wave FFT Amplitude"))) {
            DisplayPlot(I18N("Wave FFT Amplitude", "ImPlot/Wave/Wave FFT Amplitude"),
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
#if defined(VIBRATION_LOCALIZATION_ALWAYS_UPDATE) || defined(WAVEFORM_RESTORE_ALWAYS_UPDATE)
    if (err == DeviceHandler::ReadResult::OK) {
        auto& processResult = pCtx->processHandler.processResult;
#ifdef VIBRATION_LOCALIZATION_ALWAYS_UPDATE
        if (pCtx->processHandler.ProcessVibrationLocalization()) {
            pCtx->loggerHandler.LogVibration(pCtx);
        }
#endif
#ifdef WAVEFORM_RESTORE_ALWAYS_UPDATE
        pCtx->processHandler.ProcessWaveform();
#endif
    }
#endif

	if (!ImGui::CollapsingHeader(I18N("Waveform"), ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

    switch (err) {
    case DeviceHandler::ReadResult::NO_DEVICE:
        ImGui::TextUnformatted(I18N("Device not started"));
        break;
	case DeviceHandler::ReadResult::NO_CONTEXT:
        ImGui::TextUnformatted(I18N("Context not created"));
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
