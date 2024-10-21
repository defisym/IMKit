#pragma once

#include <format>
#include <algorithm>

#include "imgui.h"

template<typename T>
inline bool AddSpin(const char* label,
    T* v, T min, T max, T step = 1) {
    auto bChanged = false;

    ImGui::SameLine();
    if (ImGui::Button(std::format("-##{}", label).c_str())) {
        *v = (std::max)(*v - step, min);
        bChanged = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(std::format("+##{}", label).c_str())) {
        *v = (std::min)(*v + step, max);
        bChanged = true;
    }

    return bChanged;
}