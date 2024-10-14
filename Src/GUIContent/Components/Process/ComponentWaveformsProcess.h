#pragma once

#include "../GUIContext/Context.h"
#include "../Basic/ComponentBase.h"

struct ComponentWaveformsProcess :ComponentBase {  // NOLINT(cppcoreguidelines-special-member-functions)
	OTDRContextHandle hContext = nullptr;
	constexpr static ImVec2 PLOT_SIZE = { -1.0f, 300.0f };
	constexpr static ImGuiTabBarFlags TAB_BAR_FLAGS = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton;
    constexpr static ImGuiSliderFlags SLIDER_FLAGS = ImGuiSliderFlags_AlwaysClamp;

	using BufferHandle = void*;
	BufferHandle pWaveBuffer = nullptr;             // for processing
	BufferHandle pWaveDisplayBuffer = nullptr;      // for raw data displaying

    FilterHandle hHighPassFilter = nullptr;
    constexpr static int DEFAULT_ORDER = 5;
    constexpr static double DEFAULT_CUTOFF_FREQUENCY = 10.0;

    FilterHandle hMeanFilter = nullptr;
    constexpr static size_t DEFAULT_MEAN_RADIUS = 3;

    struct FilterParamBase {
        bool bEnable = false;
        bool bUpdate = false;
    };

    struct HighPassFilterParam :FilterParamBase {
        int filterStopFrequency = 20;
    };

    struct MeanFilterParam :FilterParamBase {
        int radius = 3;
    };

	ComponentWaveformsProcess(Ctx* p);
	~ComponentWaveformsProcess();

    // do not display more than MAX_DISPLAY_FRAME
    // for raw data -> nobody cares!
    static size_t GetDisplayFrame(const size_t frameCount);

    void WaveformTab();

    void Raw() const;
	void Shake() const;
	void Wave();

    struct ShakeInfo {
		bool diff = false;
		int shakeStart = 50;
		int shakeRange = 20;
		int unwrap2DStart = 1;
		int averageRange = 1;
	};

    struct WaveRestoreOpt :ShakeInfo {
        bool bPlayAudio = false;
        bool bUseReference = false;
        bool bReferenceAverage = false;
        int referenceStart = 50;

        HighPassFilterParam highPassFilterParam = {};
        MeanFilterParam meanFilterParam = {};
    };

    bool bOptChanged = false;
    //WaveRestoreOpt opt = {};

    [[nodiscard]] WaveRestoreOpt GetWaveRestoreOpt();
    bool WaveProcess(const WaveRestoreOpt& opt);

    std::vector<OTDRProcessValueType> audioBuffer;
    std::vector<OTDRProcessValueType> restoreWaveBuffer;
    std::vector<OTDRProcessValueType> referenceWaveBuffer;

    size_t fftElement = 0;
    std::vector<OTDRProcessValueType> restoreWaveFFTBuffer;

    void WaveRestoreProcess(OTDRProcessValueType* pProcess,
        const ShakeInfo& shakeInfo,
        std::vector<OTDRProcessValueType>& waveBuffer) const;
    void WaveRestore(OTDRProcessValueType* pProcess, const WaveRestoreOpt& opt);
    void WaveDisplay() const;
};
