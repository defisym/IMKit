#include "ComponentWaveform.h"

#include "Process/ComponentProcess.h"

void ComponentWaveform(Ctx* pCtx) {
    if (!ImGui::CollapsingHeader(I18N("Waveform"), ImGuiTreeNodeFlags_DefaultOpen)) {
        return;
    }

    ComponentProcess(pCtx);
}
