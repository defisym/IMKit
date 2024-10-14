#include "ComponentConfig.h"

#include "ComponentConfigDevice.h"
#include "ComponentConfigParam.h"

void ComponentConfig(Ctx* pCtx) {
	if (!ImGui::CollapsingHeader("Config", ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

	ImGui::TextUnformatted(pCtx->deviceHandler.supportedDevices.c_str());

	ComponentConfigDevice(pCtx);
	ComponentConfigParam(pCtx);
}
