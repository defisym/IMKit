#include "ComponentFrameRate.h"

void ComponentFrameRate(Ctx* pCtx) {
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
