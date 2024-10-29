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
                    { static_cast<int>(bufferStride) });

        ImPlot::EndPlot();
    }
    if (BeginPlotEx(I18NSTR("CH2", "ImPlot/Raw/CH2"), I18NSTR("Point"))) {
        DisplayPlot(I18N("CH2", "ImPlot/Raw/CH2/Plot"),
                    pBuffer + 1,
                    static_cast<int>(bufferFrameSize),
                    { static_cast<int>(bufferStride) });

        ImPlot::EndPlot();
    }

    ImGui::EndTabItem();
}

void VibrationLocalization(Ctx* pCtx) {
    const EmbraceHelper tabHelper = { ImGui::BeginTabItem(I18N("Vibration Localization")), ImGui::EndTabItem };
    if (!tabHelper.State()) { return; }
    const auto& deviceParams = pCtx->deviceHandler.deviceParams;
    const auto frameSize = static_cast<int>(deviceParams.pointNumProcess);
    const auto pHandler = pCtx->processHandler.pVibrationLocalizationHandler;

#ifdef SKIP_INTERNAL_POINTS
    const auto internalPoint = pCtx->deviceHandler.deviceParams.internalPoint;
#else
    const auto internalPoint = 0;
#endif

#ifndef VIBRATION_LOCALIZATION_ALWAYS_UPDATE
    pCtx->processHandler.ProcessVibrationLocalization();
#endif

    if (!pCtx->processHandler.processResult.bVibrationLocalizationProcessed) {
        ImGui::TextUnformatted(I18N("Data not enough"));
        return;
    }

    std::string xLabel =
#ifdef VIBRATION_LOCALIZATION_USE_METER
        I18NSTR("Meter");
#else
        I18NSTR("Point");
#endif
    PlotInfo plotInfo = {};
#ifdef WAVEFORM_RESTORE_USE_MILLISECOND
    plotInfo.xUpdater = [&] (const double index) {
        return index * pCtx->deviceHandler.deviceParams.resolution;
        };
#endif

#ifndef VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
    if (ImGui::BeginTabBar("Shake/Tab", TAB_BAR_FLAGS)) {
        // Handle Moving Average
        if (ImGui::BeginTabItem(I18N("Shake MA"))) {
            if (BeginPlotEx(I18N("MA", "ImPlot/Shake/MA"), xLabel.c_str())) {
                const auto maxFrameCount = GetDisplayFrame(pHandler->GetProcessor()->maFrameCount);
                for (size_t frameIdx = 0; frameIdx < maxFrameCount; frameIdx++) {
                    const std::string plotName = I18NFMT("Plot {}", frameIdx);
                    DisplayPlot(std::format("{}##ImPlot/Shake/MA/{}", plotName, plotName).c_str(),
                        pHandler->GetProcessor()->GetMovingAverageFrame(frameIdx) + internalPoint,
                        frameSize, plotInfo);
                }

                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }

        // Handle Moving Difference
        if (ImGui::BeginTabItem(I18N("Shake MD"))) {
            if (BeginPlotEx(I18N("MD", "ImPlot/Shake/MD"), xLabel.c_str())) {
                // accumulateFrameIndex
                const auto maxFrameCount = GetDisplayFrame(pHandler->GetProcessor()->mdFrameCount) - 1;

                for (size_t frameIdx = 0; frameIdx < maxFrameCount; frameIdx++) {
                    const std::string plotName = I18NFMT("Plot {}", frameIdx);
                    DisplayPlot(std::format("{}##ImPlot/Shake/MD/{}", plotName, plotName).c_str(),
                        pHandler->GetProcessor()->GetMovingDifferenceFrame(frameIdx) + internalPoint,
                        frameSize, plotInfo);
                }

                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(I18N("Shake MD Accumulate"))) {
#endif                
            if (BeginPlotEx(I18N("Vibration Localization", "ImPlot/Shake/MD/Accumulate"), xLabel.c_str())) {
#ifdef VIBRATION_LOCALIZATION_SHOW_LOGGER_THRESHOLD
                struct ThresholdData {
                    OTDRProcessValueType threshold;
                    PlotInfo* plotInfo = nullptr;
                } thresholdData = { pCtx->loggerHandler.loggerParams.threshold,&plotInfo };

                ImPlot::PlotLineG(I18N("Threshold", "ImPlot/Shake/MD/Accumulate/Threshold"),
                    [] (int idx, void* pData) {
                        const auto pThresholdData = static_cast<ThresholdData*>(pData);

                        return ImPlotPoint{ pThresholdData->plotInfo->xUpdater(idx),
                            pThresholdData->threshold };
                    }, &thresholdData, frameSize, ImPlotLineFlags_Shaded);
#endif
                DisplayPlot(I18N("Vibration Localization", "ImPlot/Shake/MD/Accumulate/Vibration Localization"),
                    pHandler->GetVibrationLocalizationResult() + internalPoint, frameSize, plotInfo);

                ImPlot::EndPlot();
            }

#ifdef VIBRATION_LOCALIZATION_LOG_WAVEFORM
            const auto pPeakHandler = pCtx->processHandler.pPeakWaveformRestoreHandler;
            const auto& peakData = pPeakHandler->GetPeakWaveformRestoreResult();

            if (BeginSubPlotEx(I18N("Peak waveform restore"), 1, 2)) {
                if (BeginPlotEx(I18N("Restore base (Vibration Localization Result)"), xLabel.c_str())) {
                    DisplayPlot(I18N("Restore base", "ImPlot/ShakePeakWaveRestore"),
                        // internal point NOT included
                        pPeakHandler->GetVibrationLocalizationData(), frameSize, plotInfo);
                    ImPlot::EndPlot();
                }
                if (BeginPlotEx(I18N("Process base (Filtered)"), xLabel.c_str())) {
                    DisplayPlot(I18N("Process base", "ImPlot/ShakePeakWaveRestore"),
                        // filtered, internal point NOT included
                        pPeakHandler->GetVibrationLocalizationFilteredData(), frameSize, plotInfo);
                    ImPlot::EndPlot();
                }
                ImPlot::EndSubplots();
            }

            if (peakData.empty()) {
                ImGui::TextUnformatted(I18N("No peak found"));
            }
            else {
                if (ImGui::BeginTabBar(I18N("Wave"), TAB_BAR_FLAGS)) {
                    for (size_t index = 0; index < peakData.size(); index++) {
                        const auto& ctx = peakData[index];
                        if (ImGui::BeginTabItem(I18NFMT("Wave {}", index))) {
                            const auto position = plotInfo.xUpdater(ctx.opt.shakeStart + ctx.opt.unwrap2DStart);
                            ComponentWaveformDisplayResult(pCtx, I18NFMT("Shake waveform at {} m", position), ctx.restore);
                            ImGui::EndTabItem();
                        }
                    }

                    ImGui::EndTabBar();
                }
            }
#endif
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
        if (BeginPlotEx(I18N("Wave Unprocessed", "ImPlot/Wave/Wave Unprocessed"), I18NSTR("Point"))) {
            for (size_t frameIdx = 0;
                frameIdx < GetDisplayFrame(deviceParams.processFrameCount);
                frameIdx++) {
                const std::string plotName = I18NFMT("Plot {}", frameIdx);
                DisplayPlot(std::format("{}##ImPlot/Wave/Wave Unprocessed/{}", plotName, plotName).c_str(),
                    Context_GetConstFrameBuffer(pHandler->GetWaveDisplay(),
                    deviceParams.pointNumPerScan, frameIdx),
                    deviceParams.pointNumPerScan);
            }

            ImPlot::EndPlot();
        }
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem(I18N("Wave Shake"))) {
#endif
        ComponentWaveformDisplayResult(pCtx, I18N("Shake waveform"), pHandler->GetRestore());
#ifndef WAVEFORM_RESTORE_ONLY_SHOW_RESULT
        ImGui::EndTabItem();
    }
#endif
}

void ComponentWaveformDisplayResult(Ctx* pCtx, const char* pTitle, const WaveformRestoreOutput& waveform) {
#ifdef WAVEFORM_RESTORE_DISPLAY_WAVE_AND_FFT_IN_SUBPLOT
    const EmbraceHelper subPlotHelper = { BeginSubPlotEx(pTitle,1,2), ImPlot::EndSubplots };
    if (!subPlotHelper.State()) { return; }
#endif

    if (BeginPlotEx(I18N("Wave Shake", "ImPlot/Wave/Wave Shake"),
#ifdef WAVEFORM_RESTORE_USE_MILLISECOND
        I18NSTR("ms")
#else
        I18NSTR("Point")
#endif
        )) {
        PlotInfo plotInfo = {};
#ifdef WAVEFORM_RESTORE_USE_MILLISECOND
        const auto& deviceParam = pCtx->deviceHandler.deviceParams;
        const auto timeScale = (1000.0 / deviceParam.scanRate);

        plotInfo.xUpdater = [&] (const double index) { return index * timeScale; };
#endif

        DisplayPlot(I18N("Wave Shake", "ImPlot/Wave/Wave Shake"),
            waveform.restore.data(),            
            static_cast<int>(waveform.restore.size()),
            plotInfo
        );

        ImPlot::EndPlot();
    }

    if (BeginPlotEx(I18N("Wave FFT Amplitude", "ImPlot/Wave/Wave FFT Amplitude"), I18NSTR("Hz"))) {
        PlotInfo plotInfo = {};
        const auto& deviceParams = pCtx->deviceHandler.deviceParams;
        plotInfo.xUpdater = [&] (const double index) {
            // use the original element size for frequency calculation
            return static_cast<double>(Util_FFT_GetFrequency(static_cast<size_t>(index),
                waveform.fft.size(),
                static_cast<float>(deviceParams.scanRate)));
            };
        DisplayPlot(I18N("Wave FFT Amplitude", "ImPlot/Wave/Wave FFT Amplitude"),
            waveform.fft.data(),
            static_cast<int>(waveform.fftElement),
            plotInfo);

        ImPlot::EndPlot();
    }
}

// Note: ImGui call this each frame
//       if move to other UI lib should change the read logic
void ComponentWaveform(Ctx* pCtx) {
    // out of tab: should always pull data
    const auto err = pCtx->deviceHandler.ReadData();
#if defined(VIBRATION_LOCALIZATION_ALWAYS_UPDATE) || defined(WAVEFORM_RESTORE_ALWAYS_UPDATE)
    if (err == DeviceHandler::ReadResult::OK) {
#ifdef VIBRATION_LOCALIZATION_ALWAYS_UPDATE
        pCtx->processHandler.ProcessVibrationLocalization();
        pCtx->loggerHandler.LogVibration(pCtx);
#endif
#ifdef WAVEFORM_RESTORE_ALWAYS_UPDATE        
        pCtx->processHandler.ProcessWaveform();
#ifdef VIBRATION_LOCALIZATION_LOG_WAVEFORM
        const auto& loggerParams = pCtx->loggerHandler.loggerParams;
        if (loggerParams.bUseThreshold) {
            if (pCtx->processHandler.ProcessPeakWaveform(loggerParams.threshold)) {
                // TODO log here
            }
        }
#endif
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
