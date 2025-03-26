#pragma once

#include "imgui.h"

bool SliderSpinInt(const char* label,
    int* v, int v_min, int v_max, int step = 1,
    const char* format = "%d", ImGuiSliderFlags flags = 0);