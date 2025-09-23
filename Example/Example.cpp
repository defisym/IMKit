#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "macro.h"

#include "IMGuiInterface/IMGuiInterface.h"
#include "IMGuiInterface/IMGuiContext.h"

#ifdef SHOW_DEMO_WINDOW
#include "IMGuiInterface/IMGuiDemo.h"
#endif

#include "GUIContent/Components/Basic/ComponentWindow.h"
#include "GUIContent/Components/Basic/ComponentPerformanceBase.h"

#ifdef USE_HIGEPERFORMANCE_GPU
extern "C" {
#ifdef USE_NVIDIA_GPU
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
#endif
#ifdef USE_AMD_GPU
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif
}
#endif

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow) {
    auto pCtx = std::make_unique<IMGUIContext>();

#ifdef _WIN64
#ifndef _DEBUG
    pCtx->pWindowName = L"IMKit x64";
#else
    pCtx->pWindowName = L"IMKit x64 DEBUG";
#endif
#else
#ifndef _DEBUG
    pCtx->pWindowName = L"IMKit";
#else
    pCtx->pWindowName = L"IMKit DEBUG";
#endif
#endif

#ifdef DISPLAY_NO_SYNC
    pCtx->bVSync = false;
#endif

    // ------------
    // GUI Loop
    // ------------
#ifdef SHOW_DEMO_WINDOW
    imguiDemo();
#else
    IMGUIInterface(pCtx.get(), [] (IMGUIContext* p) {
        ComponentWindow componentWindow = { };

#ifdef FONT_SHOW_FONT_DEBUG
        pCtx->pFont->FontDebug();
#endif

        ComponentPerformanceBase(p);
    });
#endif

    return 0;
}