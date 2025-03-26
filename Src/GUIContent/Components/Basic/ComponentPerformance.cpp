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
    {
        ImGui::TextUnformatted(I18N("Current: "));
        ImGui::SameLine();
        ImGui::Text(I18N("Read Data: %.3f ms"), performaceParams.read.cur.readTime);
        ImGui::SameLine();
        
        ImGui::TextUnformatted(I18N("Average: "));
        ImGui::SameLine();
        ImGui::Text(I18N("Read Data: %.3f ms"), performaceParams.read.avg.readTime);
        ImGui::SameLine();
     
        ImGui::TextUnformatted(I18N("Slowest: "));
        ImGui::SameLine();
        ImGui::Text(I18N("Read Data: %.3f ms"), performaceParams.read.slowest.readTime);
    }
    {
        ImGui::TextUnformatted(I18N("Current: "));
        ImGui::SameLine();
        ImGui::Text(I18N("Vibration Localization: %.3f ms, Waterfall Chat: %.3f ms, Log: %.3f ms"),
            performaceParams.vl.cur.processTime, performaceParams.vl.cur.waterfallChatTime, performaceParams.vl.cur.logTime);

        ImGui::TextUnformatted(I18N("Average: "));
        ImGui::SameLine();
        ImGui::Text(I18N("Vibration Localization: %.3f ms, Waterfall Chat: %.3f ms, Log: %.3f ms"),
            performaceParams.vl.avg.processTime, performaceParams.vl.avg.waterfallChatTime, performaceParams.vl.avg.logTime);
        
        ImGui::TextUnformatted(I18N("Slowest: "));
        ImGui::SameLine();
        ImGui::Text(I18N("Vibration Localization: %.3f ms, Waterfall Chat: %.3f ms, Log: %.3f ms"),
            performaceParams.vl.slowest.processTime, performaceParams.vl.slowest.waterfallChatTime, performaceParams.vl.slowest.logTime);
    }
    {
        ImGui::TextUnformatted(I18N("Current: "));
        ImGui::SameLine();
        ImGui::Text(I18N("Waveform Restore: %.3f ms, Peak Restore: %.3f ms, Log UI: %.3f ms, Log: %.3f ms"),
            performaceParams.wr.cur.processTime, performaceParams.wr.cur.peakProcess,
            performaceParams.wr.cur.logUITime, performaceParams.wr.cur.logTime);

        ImGui::TextUnformatted(I18N("Average: "));
        ImGui::SameLine();
        ImGui::Text(I18N("Waveform Restore: %.3f ms, Peak Restore: %.3f ms, Log UI: %.3f ms, Log: %.3f ms"),
            performaceParams.wr.avg.processTime, performaceParams.wr.avg.peakProcess,
            performaceParams.wr.avg.logUITime, performaceParams.wr.avg.logTime);

        ImGui::TextUnformatted(I18N("Slowest: "));
        ImGui::SameLine();
        ImGui::Text(I18N("Waveform Restore: %.3f ms, Peak Restore: %.3f ms, Log UI: %.3f ms, Log: %.3f ms"),
            performaceParams.wr.slowest.processTime, performaceParams.wr.slowest.peakProcess,
            performaceParams.wr.slowest.logUITime, performaceParams.wr.slowest.logTime);
    }
}
