#pragma once

#include "imgui.h"
#include "IMGuiInterface/D3DContext.h"

#include "IMGuiEx/FontEx.h"
#include "IMGuiEx/I18NInterface.h"

constexpr static ImGuiTabBarFlags TAB_BAR_FLAGS = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton;
constexpr static ImGuiSliderFlags SLIDER_FLAGS = ImGuiSliderFlags_AlwaysClamp;

struct IMGUIContext {
    const wchar_t* pWindowName = L"Dear ImGui DirectX11 Example";
    int width = 1280;
    int height = 800;
    bool bDPIAware = false;
    bool bVSync = true;
    HINSTANCE hInstance = nullptr;

    float mainLoopTime = 0;
    float guiTime = 0;
    float renderTime = 0;

    bool bDarkMode = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    UINT resizeWidth = 0;
    UINT resizeHeight = 0;
    D3DContextSwapChain renderContext = {};

    ImGuiIO* pIO = nullptr;
    FontEx* pFont = nullptr;   

    IMGUIContext() = default;
    virtual ~IMGUIContext() = default;

    IMGUIContext(const IMGUIContext& other) = default;
    IMGUIContext(IMGUIContext&& other) noexcept = default;
    IMGUIContext& operator=(const IMGUIContext& other) = default;
    IMGUIContext& operator=(IMGUIContext&& other) noexcept = default;

    // init members which need imgui context
    bool InitContext();
    bool UnInitContext();

    virtual bool CreateContext() { return true; }
    virtual bool DestroyContext() { return true; }

    // main loop, called before UI refresh
    virtual void MainLoop() {};
};