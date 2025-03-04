#include "ComponentInspect.h"

#include "macro.h"

#include "GUIContext/Handler/InspectHandler.h"
#include "IMGuiEx/IndentHelper.h"

static void ChangePage(const Ctx* pCtx, const char* pLabel) {
    ImGui::SeparatorText(I18N("ChangePage"));
    if (ImGui::Button(I18N("Prev", pLabel))) {}
    ImGui::SameLine();
    if (ImGui::Button(I18N("Next", pLabel))) {}

    ImGui::SeparatorText(I18N("Waveform"));
}

void ComponentInspect(const Ctx* pCtx) {
    if (ImGui::BeginTabBar("Inspect/Tab", TAB_BAR_FLAGS)) {
        if (ImGui::BeginTabItem(I18N("Vibration", "Inspect"))) {
#ifdef INDENT_INSIDE_TAB
            IndentHelper helper = {};
#endif
            ChangePage(pCtx, "Vibration");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(I18N("Waveform", "Inspect"))) {
#ifdef INDENT_INSIDE_TAB
            IndentHelper helper = {};
#endif
            ChangePage(pCtx, "Waveform");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}
