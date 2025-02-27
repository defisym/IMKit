#include "ComponentFrameRate.h"

void ComponentFrameRate(Ctx* pCtx) {    
    ImGui::Text(I18N("Application average %.3f ms/frame (%.1f FPS)"),
         1000.0f / pCtx->pIO->Framerate, pCtx->pIO->Framerate);
    ImGui::SameLine();

    const auto ms = 1000.0f * pCtx->pIO->DeltaTime;
    ImGui::Text(I18N("Last frame: %.3f ms"), ms);
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

    auto& performaceParams = pCtx->performanceParams;

#ifdef MULTITHREAD
    const auto helper = performaceParams.GetUILockHelper();
#endif

    ImGui::Text(I18N("Read Data: %.3f ms, Vibration Localization: %.3f ms, Waveform Restore: %.3f ms"),
       performaceParams.readTime, performaceParams.vibrationLocalizationTime, performaceParams.waveformRestoreTime);
}
