#pragma once

#include "../GUIContext/Context.h"

#include "ComponentBase.h"

struct ComponentWaveformsProcess :ComponentBase {  // NOLINT(cppcoreguidelines-special-member-functions)
	OTDRContextHandle hContext = nullptr;
	constexpr static ImVec2 plotSize = { -1.0f, 300.0f };
	constexpr static ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton;
    constexpr static ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;

	using BufferHandle = void*;
	BufferHandle pWaveBuffer = nullptr;             // for processing
	BufferHandle pWaveDisplayBuffer = nullptr;      // for raw data displaying

	ComponentWaveformsProcess(Ctx* p);
	~ComponentWaveformsProcess();

	void WaveformTab() {
		if (!ImGui::BeginTabBar("Waveforms/Tab", tabBarFlags)) { return; }

		this->Raw();
		this->Shake();
		this->Wave();

		ImGui::EndTabBar();
	}

	void Raw() const;
	void Shake() const;
	void Wave();

    struct ShakeInfo {
		bool diff = false;
		int shakeStart = 50;
		int shakeRange = 20;
		int unwrap2DStart = 1;
	};

    struct WaveRestoreOpt :ShakeInfo {
        bool bUseReference = false;
        int referenceStart = 50;
        bool bPlayAudio = false;
    };

    [[nodiscard]] WaveRestoreOpt GetWaveRestoreOpt() const;
    bool WaveProcess(const WaveRestoreOpt& opt);

    std::vector<OTDRProcessValueType> restoreWaveBuffer;
    std::vector<OTDRProcessValueType> restoreWaveFFTBuffer;
    std::vector<OTDRProcessValueType> referenceWaveBuffer;

    void WaveRestoreProcess(OTDRProcessValueType* pProcess,
        const ShakeInfo& shakeInfo,
        std::vector<OTDRProcessValueType>& waveBuffer) const;
    void WaveRestore(OTDRProcessValueType* pProcess, const WaveRestoreOpt& opt);
    void WaveDisplay() const;
};
