#include "ComponentWaveform.h"

#include "macro.h"

#include "Process/ComponentProcess.h"
#include "Inspect/ComponentInspect.h"
#include "Logger/ComponentLogger.h"
#ifdef INDENT_INSIDE_TAB
#include "IMGuiEx/IndentHelper.h"
#endif

void ComponentWaveform(Ctx* pCtx) {
    if (!ImGui::CollapsingHeader(I18N("Waveform"), ImGuiTreeNodeFlags_DefaultOpen)) {
        return;
    }

    if (ImGui::BeginTabBar("Waveform/Tab", TAB_BAR_FLAGS)) {
        if (ImGui::BeginTabItem(I18N("Process"))) {
#ifdef INDENT_INSIDE_TAB
            IndentHelper helper = {};
#endif
            ComponentProcess(pCtx);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(I18N("Log"))) {
#ifdef INDENT_INSIDE_TAB
            IndentHelper helper = {};
#endif
            ComponentLogger(pCtx);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(I18N("Inspect"))) {
#ifdef INDENT_INSIDE_TAB
            IndentHelper helper = {};
#endif
            ComponentInspect(pCtx);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}
