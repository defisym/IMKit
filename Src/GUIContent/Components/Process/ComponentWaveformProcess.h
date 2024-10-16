#pragma once

#include "../GUIContext/Context.h"
#include "../Basic/ComponentBase.h"

struct ComponentWaveformProcess :ComponentBase {  // NOLINT(cppcoreguidelines-special-member-functions)
    ComponentWaveformProcess(Ctx* p);

    // do not display more than MAX_DISPLAY_FRAME
    // for raw data -> nobody cares!
    static size_t GetDisplayFrame(const size_t frameCount);

    void WaveformTab() const;
    void Raw() const;
	void Shake() const;
	void Wave() const;
    void WaveDisplay() const;
};
