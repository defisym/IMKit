#pragma once

#define NOMINMAX
#include <d3d11.h>

#include "IMGuiEx/FontEx.h"
#include "IMGuiEx/LabelMaker.h"
#include "Internationalization/Internationalization.h"

struct D3DContext {
    ID3D11Device* pD3DDevice = nullptr;
    ID3D11DeviceContext* pD3DDeviceContext = nullptr;
    IDXGISwapChain* pSwapChain = nullptr;
    UINT resizeWidth = 0;
    UINT resizeHeight = 0;
    ID3D11RenderTargetView* pRenderTargetView = nullptr;
};

constexpr static ImVec2 PLOT_SIZE = { -1.0f, 300.0f };
constexpr static ImGuiTabBarFlags TAB_BAR_FLAGS = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton;
constexpr static ImGuiSliderFlags SLIDER_FLAGS = ImGuiSliderFlags_AlwaysClamp;

struct IMGUIContext {
    const wchar_t* pWindowName = L"Dear ImGui DirectX11 Example";
    int width = 1280;
    int height = 800;
    bool bDPIAware = false;

    bool bDarkMode = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGuiIO* pIO = nullptr;
    FontEx* pFont = nullptr;
    I18N i18n = {};
    LabelMaker labelMaker = { this };

    const char* GetI18NLabel(const char* displayName);
    const char* GetI18NLabel(const char* displayName, const char* label);

    D3DContext renderContext = {};
    
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
};