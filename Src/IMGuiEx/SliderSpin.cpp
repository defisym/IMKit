#include "SliderSpin.h"

#include "AddSpin.h"

bool SliderSpinInt(const char* label,
                   int* v, int v_min, int v_max, int step,
                   const char* format, const ImGuiSliderFlags flags) {
    bool bUpdate = false;

    bUpdate |= ImGui::SliderInt(std::format("##{}", label).c_str(),
        v, v_min, v_max, format, flags);
    bUpdate |= AddSpin(label, v, v_min, v_max, step);

    return bUpdate;
}
