#include "ComponentWaveform.h"

#include "ComponentWaveformProcess.h"

// Note: ImGui call this each frame
//       if move to other UI lib should change the read logic
void ComponentWaveform(Ctx* pCtx) {
    // out of tab: should always pull data
    const auto err = pCtx->deviceHandler.ReadData();

	if (!ImGui::CollapsingHeader("Waveform", ImGuiTreeNodeFlags_DefaultOpen)) {
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
        ComponentWaveformProcess(pCtx).WaveformTab();
    }
}
