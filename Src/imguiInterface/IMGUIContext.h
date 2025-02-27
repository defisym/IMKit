#pragma once

#define NOMINMAX
#include <d3d11.h>
#include <GeneralDefinition.h>

#include "imgui/imgui.h"

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

constexpr static ImGuiTabBarFlags TAB_BAR_FLAGS = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton;
constexpr static ImGuiSliderFlags SLIDER_FLAGS = ImGuiSliderFlags_AlwaysClamp;

#define I18N(...) pCtx->GetI18NLabel(__VA_ARGS__)
#define I18NFMT(...) pCtx->GetI18NLabelFMT(__VA_ARGS__)

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
    Internationalization i18n = {};
    LabelMaker labelMaker = { this };

    StringResult GetI18NLabel(const char* displayName) const;
    StringResult GetI18NLabel(const char* displayName, const char* label) const;

    template <class... Types>
    StringResult GetI18NLabelFMT(const char* displayName, Types&&... args) {
        const auto fmt = std::vformat(to_wide_string(labelMaker.MakeLabel(displayName)),
            std::make_wformat_args(std::forward<Types>(args)...));
        return to_byte_string(fmt);
    }
    template <class... Types>
    StringResult GetI18NLabelFMT(const char* displayName, const char* label, Types&&... args) {
        const auto fmt = GetI18NLabelFMT(displayName, std::forward<Types>(args)...);
        return LabelMaker::ConnectLabel(fmt, label);
    }

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