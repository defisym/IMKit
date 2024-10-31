#pragma once

#define NOMINMAX
#include <d3d11.h>
#include <_DeLib/GeneralDefinition.h>

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

#define I18N pCtx->GetI18NLabel
#define I18NFMT pCtx->GetI18NLabelFMT
#define I18NSTR(...) pCtx->GetI18NLabelStr(__VA_ARGS__).c_str()

struct IMGUIContext {
    const wchar_t* pWindowName = L"Dear ImGui DirectX11 Example";
    int width = 1280;
    int height = 800;
    bool bDPIAware = false;
    HINSTANCE hInstance = nullptr;

    bool bDarkMode = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    D3DContext renderContext = {};

    ImGuiIO* pIO = nullptr;
    FontEx* pFont = nullptr;
    Internationalization i18n = {};
    LabelMaker labelMaker = { this };

    const char* GetI18NLabel(const char* displayName);
    const char* GetI18NLabel(const char* displayName, const char* label);
    std::string GetI18NLabelStr(const char* displayName) const;
    std::string GetI18NLabelStr(const char* displayName, const char* label) const;

    template <class... Types>
    const char* GetI18NLabelFMT(const char* displayName, Types&&... args) {
        const auto fmt = std::vformat(to_wide_string(labelMaker.MakeLabel(displayName)),
            std::make_wformat_args(args...));
        return labelMaker.UpdateCache(to_byte_string(fmt)).c_str();
    }
    template <class... Types>
    const char* GetI18NLabelFMT(const char* displayName, const char* label, Types&&... args) {
        const auto fmt = GetI18NLabelFMT(displayName, args...);
        return labelMaker.UpdateCache(LabelMaker::ConnectLabel(fmt, label)).c_str();
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
};