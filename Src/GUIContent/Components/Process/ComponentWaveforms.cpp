#include "ComponentWaveforms.h"

#include "ComponentWaveformsProcess.h"

// Note: ImGui call this each frame
//       if move to other UI lib should change the read logic
void ComponentWaveforms(Ctx* pCtx) {
    // out of tab: should always pull data
    const auto err = pCtx->deviceHandler.ReadData();

	if (!ImGui::CollapsingHeader("Waveforms", ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

    switch (err) {
    case DeviceHandler::ReadResult::NO_DEVICE:
        ImGui::TextUnformatted("Device not started");
        break;
	case DeviceHandler::ReadResult::NO_CONTEXT:
        ImGui::TextUnformatted("Context not created");
        break;
	case DeviceHandler::ReadResult::OK:
        ComponentWaveformsProcess(pCtx).WaveformTab();
    }
}
