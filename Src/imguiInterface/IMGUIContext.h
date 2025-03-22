#pragma once

#include "macro.h"

#define NOMINMAX
#include <d3d11.h>
#ifdef MULTITHREAD
#include <d3d11_4.h>
#endif

#include "imgui/imgui.h"

#include "IMGuiEx/FontEx.h"
#include "IMGuiEx/I18NInterface.h"

struct D3DContext {
    ID3D11Device* pD3DDevice = nullptr;
    ID3D11DeviceContext* pD3DDeviceContext = nullptr;
#ifdef MULTITHREAD
    ID3D11Multithread* pD3D11Multithread = nullptr;
#endif
    IDXGISwapChain* pSwapChain = nullptr;
    UINT resizeWidth = 0;
    UINT resizeHeight = 0;
    ID3D11RenderTargetView* pRenderTargetView = nullptr;
};

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

    bool bDarkMode = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    D3DContext renderContext = {};

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