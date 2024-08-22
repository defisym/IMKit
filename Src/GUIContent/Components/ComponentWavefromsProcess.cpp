#include "ComponentWavefromsProcess.h"

#include "../../IMGuiEx/DisplayPlot.h"
#include "../../IMGuiEx/AddSpin.h"
#include "../../IMGuiEx/DisableHelper.h"

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

	const auto pProcess = Context_GetProcessBuffer(hContext, 1);

	if (ImGui::BeginTabBar("Shake/Tab", tab_bar_flags)) {
		auto frameCount = bufferInfo.frameCount;

		frameCount = Util_MovingAverage(pProcess,
										frameCount, bufferInfo.frameSize,
										processParams.movingAvgRange);
		if (ImGui::BeginTabItem("Shake MA")) {
			if (ImPlot::BeginPlot("ImPlot/Shake/MA", plotSize)) {
				for (size_t frameIdx = 0; frameIdx < frameCount; frameIdx++) {
					DisplayPlot(std::format("ImPlot/Shake/MA/Plot_{}", frameIdx).c_str(),
								Context_GetFrameBuffer(pProcess,
						bufferInfo.frameSize, frameIdx),
								deviceParams.pointNumPerScan);
				}

				ImPlot::EndPlot();
			}
			ImGui::EndTabItem();
		}

		frameCount = Util_MovingDifference(pProcess,
										   frameCount, bufferInfo.frameSize,
										   processParams.movingDiffRange);
		if (ImGui::BeginTabItem("Shake MD")) {
			if (ImPlot::BeginPlot("ImPlot/Shake/MD", plotSize)) {
				for (size_t frameIdx = 0; frameIdx < frameCount - 1; frameIdx++) {
					DisplayPlot(std::format("ImPlot/Shake/MD/Plot_{}", frameIdx).c_str(),
								Context_GetFrameBuffer(pProcess,
						bufferInfo.frameSize, frameIdx),
								deviceParams.pointNumPerScan);
				}

				ImPlot::EndPlot();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Shake MD Accumulate")) {
			if (ImPlot::BeginPlot("ImPlot/Shake/MD/Accumulate", plotSize)) {
				ImPlot::PlotLine("ImPlot/Shake/MD/Accumulate/Plot",
								 Context_GetFrameBuffer(pProcess,
					bufferInfo.frameSize, frameCount - 1),
								 deviceParams.pointNumPerScan);
				ImPlot::EndPlot();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::EndTabItem();
}

void ComponentWavefromsProcess::Wave() {
	if (ImGui::BeginTabItem("Wave")) {
		const auto pProcess = Context_GetProcessBuffer(hContext, 0);

		if (ImGui::BeginTabBar("Wave/Tab", tab_bar_flags)) {

			this->WaveNormalization(pProcess);

			if (ImGui::BeginTabItem("Wave Shake")) {
				this->WaveRestore(pProcess);

				ImGui::EndTabItem();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void ComponentWavefromsProcess::WaveNormalization(OTDRProcessValueType* pProcess) const {
	const auto& bufferInfo = pCtx->deviceHandler.bufferInfo;
	const auto& deviceParams = pCtx->deviceParams;

	OTDRProcessValueType offset = 0.0f;
	OTDRProcessValueType scale = 0.0f;
	Util_WaveNormalization(pProcess, bufferInfo.bufferSz, &offset, &scale);

	for (size_t idx = 0; idx < bufferInfo.bufferSz; idx++) {
		pProcess[idx] *= PI;
	}

	if (ImGui::BeginTabItem("Wave Unprocessed")) {
		if (ImPlot::BeginPlot("ImPlot/Wave/Wave Unprocessed", plotSize)) {
			for (size_t frameIdx = 0; frameIdx < bufferInfo.frameCount; frameIdx++) {
				DisplayPlot(std::format("ImPlot/Wave/Wave Unprocessed/Plot_{}", frameIdx).c_str(),
							Context_GetFrameBuffer(pProcess,
					bufferInfo.frameSize, frameIdx),
							deviceParams.pointNumPerScan);
			}

			ImPlot::EndPlot();
		}
		ImGui::EndTabItem();
	}

}

void ComponentWavefromsProcess::WaveRestore(OTDRProcessValueType* pProcess) {
	const auto& bufferInfo = pCtx->deviceHandler.bufferInfo;
	const auto& deviceParams = pCtx->deviceParams;

	// TODO find the possible shake start position
	static ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;

	// ------------------------------------
	// Reference
	// ------------------------------------

	static bool reference = false;
	ImGui::Checkbox("use reference", &reference);
	ImGui::SameLine();
	ImGui::TextUnformatted("reference shares the range & unwrap settings");

	auto disableReference = ManualDisableHelper();

	disableReference.Disable(!reference);

	static int referenceStart = 50;
	ImGui::SliderInt("##reference start", &referenceStart,
					 0, static_cast<int>(bufferInfo.frameSize),
					 "%d", flags);
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

	static int shakeStart = 50;
	ImGui::SliderInt("##shake start", &shakeStart,
					 0, static_cast<int>(bufferInfo.frameSize),
					 "%d", flags);
	AddSpin("shake start", &shakeStart,
			0, static_cast<int>(bufferInfo.frameSize));
	ImGui::SameLine();
	ImGui::TextUnformatted("shake start");

	static int shakeRange = 20;
	ImGui::SliderInt("##shake range", &shakeRange,
					 0, static_cast<int>(bufferInfo.frameSize) - shakeStart,
					 "%d", flags);
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
					 "%d", flags);
	AddSpin("unwrap 2D start", &unwrap2DStart,
			1, shakeRange);
	ImGui::SameLine();
	ImGui::TextUnformatted("unwrap 2D start");

	// ------------------------------------
	// Wave Restore
	// ------------------------------------
	this->WaveRestoreProcess(pProcess,
		{ diff,shakeStart,shakeRange,unwrap2DStart },
		restoreWaveBuffer);

	// remove system noise by reference point
	if(reference) {
		this->WaveRestoreProcess(pProcess,
			{ diff,referenceStart,shakeRange,unwrap2DStart },
			referenceWaveBuffer);

		for (size_t index = 0; index < referenceWaveBuffer.size(); index++) {
			restoreWaveBuffer[index] -= referenceWaveBuffer[index];
		}
	}

	if (ImPlot::BeginPlot("ImPlot/Wave/Wave Shake", plotSize)) {
		DisplayPlot(std::format("ImPlot/Wave/Wave Shake").c_str(),
					restoreWaveBuffer.data(), static_cast<int>(restoreWaveBuffer.size()));

		ImPlot::EndPlot();
	}

	// ------------------------------------
	// Audio
	// ------------------------------------
	bool bPlayAudio = false;
	ImGui::Checkbox("ImPlot/Wave/Play Wave", &bPlayAudio);

	if (bPlayAudio) {
		memcpy(pCtx->audioHandler.pBuffer,
			restoreWaveBuffer.data(),
			sizeof(OTDRProcessValueType) * restoreWaveBuffer.size());
	}

	// ------------------------------------
	// FFT
	// ------------------------------------
	this->WaveFFT(restoreWaveBuffer);
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

void ComponentWavefromsProcess::WaveFFT(std::vector<OTDRProcessValueType>& waveBuffer) const {
	const auto& deviceParams = pCtx->deviceParams;
	const auto fftElement = Util_FFT_Amplitude(waveBuffer.data(), waveBuffer.size());

	if (ImPlot::BeginPlot("ImPlot/Wave/Wave FFT Amplitude", plotSize)) {
		DisplayPlot(std::format("ImPlot/Wave/Wave FFT Amplitude").c_str(),
					waveBuffer.data(), static_cast<int>(waveBuffer.size()), 1,
					[&] (const double index) {
						return static_cast<double>(Util_FFT_GetFrequency(static_cast<size_t>(index),
							waveBuffer.size(), static_cast<float>(deviceParams.scanRate)));
					});

		ImPlot::EndPlot();
	}
}
