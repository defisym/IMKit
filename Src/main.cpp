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

// TODO WaveRestore
//  1. obvious interference every each second -> why?
//  2. 800 ~ 1000 Hz interference, is it related to center freq or sample freq
//      2.1 Adaptive Filter?
//      2.2 calculate FFT for reference and direct remove it from main signal?
//  3. loop for a small part if context enabled

// TODO Map
// Reference:
// https://github.dev/epezent/implot_demos/blob/master/demos/maps.cpp
// Download map and cache a set of coords, for display the line info

// TODO Features
//  1. Zoom the waveform
//      1.1 the context menu should be internationalized
//      1.2 fork the implot and add callback for display
//      1.3 disable display threshold or optimize it
//  2. review the log data
//      2.1 organized in yy-mm-dd hh-mm-ss
//      2.2 link the vibration and waveform
//          2.2.1 don't need to log vibration, only log waveform 
//                and the process time vibration
//          2.2.2 or both, can be viewed in different mode
//      2.3 review data doesn't affect sampler

// TODO Known issues
//  1. sometimes device start fails -> wait or reboot
//  2. sometimes device returns NAN -> reboot
//      2.1 clear the buffer first before actual reading?