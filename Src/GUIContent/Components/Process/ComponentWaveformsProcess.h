#pragma once

#include "../GUIContext/Context.h"
#include "../Basic/ComponentBase.h"

struct ComponentWaveformsProcess :ComponentBase {  // NOLINT(cppcoreguidelines-special-member-functions)
    ComponentWaveformsProcess(Ctx* pCtx);

    // do not display more than MAX_DISPLAY_FRAME
    // for raw data -> nobody cares!
    static size_t GetDisplayFrame(const size_t frameCount);

    void WaveformTab();
    void Raw() const;
	void Shake() const;
	void Wave();

    bool bOptChanged = false;
    [[nodiscard]] WaveformsRestoreHandler::WaveRestoreOpt GetWaveRestoreOpt();

    void WaveDisplay() const;
};
