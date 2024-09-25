#pragma once

#include <functional>

#include "imgui.h"

struct GUIContext {
    const wchar_t* pWindowName = L"Dear ImGui DirectX11 Example";
    int width = 1280;
    int height = 800;
    bool bDPIAware = false;

    ImGuiIO* pIO = nullptr;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    GUIContext() = default;
    virtual ~GUIContext() = default;

    GUIContext(const GUIContext& other) = default;
    GUIContext(GUIContext&& other) noexcept = default;
    GUIContext& operator=(const GUIContext& other) = default;
    GUIContext& operator=(GUIContext&& other) noexcept = default;

    virtual bool CreateContext() { return true; }
    virtual bool DestroyContext() { return true; }
};

int ImguiInterface(GUIContext* pCtx,
	const std::function<void(GUIContext*)>& gui);
