#include "ComponentFrameRate.h"

void ComponentFrameRate(Ctx* pCtx) {    
    ImGui::Text(pCtx->GetI18NLabel("Application average %.3f ms/frame (%.1f FPS)"),
    1000.0f / pCtx->pIO->Framerate,
    pCtx->pIO->Framerate);
}
