#pragma once

#include <format>
#include <algorithm>

#include "imgui.h"

template<typename T>
inline void AddSpin(const char* label,
    T* v, T min, T max, T step = 1) {
    ImGui::SameLine();
    if (ImGui::Button(std::format("-##{}", label).c_str())) {
        *v = (std::max)(*v - step, min);
    }
    ImGui::SameLine();
    if (ImGui::Button(std::format("+##{}", label).c_str())) {
        *v = (std::min)(*v + step, max);
    }
}