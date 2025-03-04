#include "ComponentWaveform.h"

#include "Process/ComponentProcess.h"
#include "Inspect/ComponentInspect.h"
#include "Logger/ComponentLogger.h"

void ComponentWaveform(Ctx* pCtx) {
    if (!ImGui::CollapsingHeader(I18N("Waveform"), ImGuiTreeNodeFlags_DefaultOpen)) {
        return;
    }

    if (ImGui::BeginTabBar("Waveform/Tab", TAB_BAR_FLAGS)) {
        if (ImGui::BeginTabItem(I18N("Process"))) {
            ComponentProcess(pCtx);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(I18N("Log"))) {
            ComponentLogger(pCtx);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(I18N("Inspect"))) {
            ComponentInspect(pCtx);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}
