#include "DisableHelper.h"

#include "imgui_internal.h"

DisableHelper::DisableHelper(const bool bDisabled) {
    if (!bDisabled) { return; }

    this->bDisabled = bDisabled;

    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
}

DisableHelper::~DisableHelper() {
    if (!bDisabled) { return; }

    ImGui::PopItemFlag();
    ImGui::PopStyleVar();
}

void ManualDisableHelper::Disable(const bool bDisabled) {
    pDisableHelper = new DisableHelper(bDisabled);
}

void ManualDisableHelper::Enable() {
    delete pDisableHelper;
    pDisableHelper = nullptr;
}
