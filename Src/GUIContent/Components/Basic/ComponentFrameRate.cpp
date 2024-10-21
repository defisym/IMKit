#include "ComponentFrameRate.h"

void ComponentFrameRate(const Ctx* pCtx) {    
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		1000.0f / pCtx->pIO->Framerate,
        pCtx->pIO->Framerate);
}
