#pragma once

#include "imgui.h"

bool SliderSpinInt(const char* label,
    int* v, int v_min, int v_max, int step = 1,
    const char* format = "%d", ImGuiSliderFlags flags = 0);

bool SliderSpinFloat(const char* label,
    float* v, float v_min, float v_max, float step = 0.1,
    const char* format = "%.3f", ImGuiSliderFlags flags = 0);
