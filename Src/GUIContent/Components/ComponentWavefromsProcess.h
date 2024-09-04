#pragma once

#include "../GUIContext/Context.h"

#include "ComponentBase.h"

struct ComponentWavefromsProcess :ComponentBase {  // NOLINT(cppcoreguidelines-special-member-functions)
	OTDRContextHandle hContext = nullptr;
	constexpr static ImVec2 plotSize = { -1.0f, 300.0f };
	constexpr static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton;

	VibrationLocalizationContextHandle hVibrationLocalization = nullptr;

	using BufferHandle = void*;
	BufferHandle pAudioBuffer = nullptr;

	ComponentWavefromsProcess(Ctx* p, const OTDRContextHandle h);

	~ComponentWavefromsProcess();

	void StartProcess() const {
		Context_Update(hContext, &pCtx->deviceHandler.bufferInfo);
		pCtx->audioHandler.ResetBuffer();
	}
	void EndProcess() const {
		if (!pCtx->deviceHandler.bContextUpdated) { return; }

		// context updated, add new audio data
		const auto& deviceParams = pCtx->deviceParams;

		pCtx->audioHandler.audioPlayer.AddData(pCtx->audioHandler.audioData,
			{ pCtx->audioHandler.pBuffer,
				pCtx->audioHandler.bufferSz,
				static_cast<size_t>((1000.0 * deviceParams.processFrameCount) / deviceParams.scanRate) });

	}

	void WaveformTab() {
		if (!ImGui::BeginTabBar("Wavefroms/Tab", tab_bar_flags)) { return; }

		this->Raw();
		this->Shake();
		this->Wave();

		ImGui::EndTabBar();
	}

	void Raw() const;
	void Shake() const;
	void Wave();
	void WaveNormalization(OTDRProcessValueType* pProcess) const;
	void WaveRestore(OTDRProcessValueType* pProcess);

	struct ShakeInfo {
		bool diff = false;
		int shakeStart = 50;
		int shakeRange = 20;
		int unwrap2DStart = 1;
	};

	std::vector<OTDRProcessValueType> restoreWaveBuffer;
	std::vector<OTDRProcessValueType> referenceWaveBuffer;

	void WaveRestoreProcess(OTDRProcessValueType* pProcess,
		const ShakeInfo& shakeInfo,
		std::vector<OTDRProcessValueType>& waveBuffer) const;

	void WaveFFT(std::vector<OTDRProcessValueType>& waveBuffer) const;
};
