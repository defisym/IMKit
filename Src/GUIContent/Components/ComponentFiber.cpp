#include "ComponentFiber.h"

void ComponentFiber(Ctx* pCtx) {
	if (!ImGui::CollapsingHeader("Fiber", ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

	auto& params = pCtx->fiberParams;
	ImGui::InputDouble("Light speed (m/s)", &params.lightSpeed);
	ImGui::InputDouble("Length (m)", &params.length);
}
