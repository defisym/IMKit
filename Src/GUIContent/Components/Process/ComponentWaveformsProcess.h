#pragma once

#include "../GUIContext/Context.h"
#include "../Basic/ComponentBase.h"

struct ComponentWaveformsProcess :ComponentBase {  // NOLINT(cppcoreguidelines-special-member-functions)
    ComponentWaveformsProcess(Ctx* pCtx);

    // do not display more than MAX_DISPLAY_FRAME
    // for raw data -> nobody cares!
    static size_t GetDisplayFrame(const size_t frameCount);

    void WaveformTab() const;
    void Raw() const;
	void Shake() const;
	void Wave() const;

    static bool GetWaveRestoreOpt(const Ctx* pCtx, WaveformsRestoreHandler::WaveRestoreOpt& opt);

    void WaveDisplay() const;
};
