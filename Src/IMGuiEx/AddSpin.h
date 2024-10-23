#pragma once

#include <format>
#include <algorithm>

#include "imgui.h"

template<typename T>
inline bool AddSpin(const char* label,
    T* v, T min, T max, T step = 1) {
    auto bChanged = false;
    const float button_size = ImGui::GetFrameHeight();

    ImGui::SameLine();
    if (ImGui::Button(std::format("-##{}", label).c_str(), ImVec2(button_size, button_size))) {
        *v = (std::max)(*v - step, min);
        bChanged = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(std::format("+##{}", label).c_str(), ImVec2(button_size, button_size))) {
        *v = (std::min)(*v + step, max);
        bChanged = true;
    }

    return bChanged;
}