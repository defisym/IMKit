#include "ComponentPerformance.h"

void ComponentPerformance(Ctx* pCtx) {    
    if (!ImGui::CollapsingHeader(I18N("Performance"),
        ImGuiTreeNodeFlags_DefaultOpen)) {
        return;
    }

    ImGui::Text(I18N("Application average %.3f ms/frame (%.1f FPS)"),
         1000.0f / pCtx->pIO->Framerate, pCtx->pIO->Framerate);
    ImGui::SameLine();

    const auto ms = 1000.0f * pCtx->pIO->DeltaTime;
    ImGui::Text(I18N("Last frame: %.3f ms"), ms);
    ImGui::SameLine();

    ImGui::Text(I18N("Main loop: %.3f ms, GUI: %.3f ms, Render: %.3f ms"),
       pCtx->mainLoopTime, pCtx->guiTime, pCtx->renderTime);
    ImGui::SameLine();

    const auto other = ms - (pCtx->mainLoopTime + pCtx->guiTime + pCtx->renderTime);
    if (pCtx->bVSync) {
        ImGui::Text(I18N("V-sync wait & other process: %.3f ms"), other);
    }else {
        ImGui::Text(I18N("Other process: %.3f ms"), other);
    }

    auto& performaceParams = pCtx->performanceParams;
#ifdef MULTITHREAD
    const auto helper = performaceParams.GetUILockHelper();
#endif

    auto displayPerformanceParam = [] (const PerformanceParams::ReadPerformance& read,
        const PerformanceParams::VibrationLocalizationPerformance& vl,
        const PerformanceParams::WaveformRestorePerformance& wr) {
            ImGui::Text(I18N("Read Data: %.3f ms"), read.readTime);
            ImGui::SameLine();
            ImGui::Text(I18N("Vibration Localization: %.3f ms, Waterfall Chat: %.3f ms, Log: %.3f ms"),
                vl.processTime, vl.waterfallChatTime, vl.logTime);
            ImGui::SameLine();
            ImGui::Text(I18N("Waveform Restore: %.3f ms, Peak Restore: %.3f ms, Log UI: %.3f ms, Log: %.3f ms"),
                wr.processTime, wr.peakProcess, wr.logUITime, wr.logTime);
        };

        ImGui::TextUnformatted(I18N("Current:"));
        ImGui::SameLine();
        displayPerformanceParam(performaceParams.read.cur, performaceParams.vl.cur, performaceParams.wr.cur);
        
        ImGui::TextUnformatted(I18N("Average:"));
        ImGui::SameLine();
        displayPerformanceParam(performaceParams.read.avg, performaceParams.vl.avg, performaceParams.wr.avg);

        ImGui::TextUnformatted(I18N("Slowest:"));
        ImGui::SameLine();
        displayPerformanceParam(performaceParams.read.slowest, performaceParams.vl.slowest, performaceParams.wr.slowest);

        if (ImGui::Button(I18N("Reset"))) { performaceParams = {}; }
}
