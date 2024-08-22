#pragma once

#include "imgui_internal.h"

// RAII
struct DisableHelper {  // NOLINT(cppcoreguidelines-special-member-functions)
    bool bDisabled = false;

    explicit DisableHelper(const bool bDisabled) {
        if (!bDisabled) { return; }

        this->bDisabled = bDisabled;

        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    ~DisableHelper() {
        if (!bDisabled) { return; }

        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
};


struct ManualDisableHelper {  // NOLINT(cppcoreguidelines-special-member-functions)
    DisableHelper* pDisableHelper = nullptr;
    ~ManualDisableHelper() { Enable(); }

    void Disable(const bool bDisabled) {
        pDisableHelper = new DisableHelper(bDisabled);
    }
    void Enable() {
        delete pDisableHelper;
        pDisableHelper = nullptr;
    }
};
