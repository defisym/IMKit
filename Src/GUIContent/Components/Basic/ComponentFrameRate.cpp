#include "ComponentFrameRate.h"

void ComponentFrameRate(Ctx* pCtx) {
    const auto ms = 1000.0f / pCtx->pIO->Framerate;
    ImGui::Text(I18N("Application average %.3f ms/frame (%.1f FPS)"),
        ms, pCtx->pIO->Framerate);
    ImGui::SameLine();

    ImGui::Text(I18N("Main loop: %.3f ms, GUI: %.3f ms"),
       pCtx->mainLoopTime, pCtx->guiTime);
    ImGui::SameLine();

    const auto other = ms - (pCtx->mainLoopTime + pCtx->guiTime);
    if (pCtx->bVSync) {
        ImGui::Text(I18N("V-sync wait & other process: %.3f ms"), other);
    }else {
        ImGui::Text(I18N("Other process: %.3f ms"), other);
    }
}
