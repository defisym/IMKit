#pragma once

#include "GUIContext/GUIContext.h"

// display waveform and it's FFT
void ComponentWaveformDisplay(Ctx* pCtx,
    const WaveformRestoreHandler::WaveformRestoreOutput& waveform);

void ComponentWaveform(Ctx* pCtx);