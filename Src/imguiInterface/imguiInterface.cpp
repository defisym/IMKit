#include "IMGUIInterface.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <chrono>

#include <tchar.h>
#include <resource.h>

#include "Utilities/MeasureHelper.h"

// ------------------------------------------------------------
// Dear ImGui: standalone application for DirectX 11
// ------------------------------------------------------------

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// ------------------------------------------------------------
// Forward declarations of helper functions
// ------------------------------------------------------------

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ------------------------------------------------------------
// Main code
// ------------------------------------------------------------

int IMGUIInterface(IMGUIContext* pCtx,
    const std::function<void(IMGUIContext*)>& gui) {
    // Create application window
    if (pCtx->bDPIAware) {
        ImGui_ImplWin32_EnableDpiAwareness();
    }

    pCtx->hInstance = GetModuleHandle(nullptr);
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC,
        WndProc, 0L, 0L,
        pCtx->hInstance,
        LoadIcon(pCtx->hInstance, MAKEINTRESOURCE(IDI_ICON)),
        nullptr, nullptr, nullptr,
        L"ImGui Example", nullptr };
    RegisterClassExW(&wc);

    HWND hwnd = ::CreateWindowW(wc.lpszClassName,
        pCtx->pWindowName,
        WS_OVERLAPPEDWINDOW,
        100, 100,
        pCtx->width, pCtx->height,
        nullptr, nullptr, wc.hInstance, pCtx);

    // Initialize Direct3D  
    if (FAILED(pCtx->renderContext.CreateContext(hwnd))) {
        pCtx->renderContext.DestroyContext();
        UnregisterClassW(wc.lpszClassName, wc.hInstance);

        return 1;
    }

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    pCtx->CreateContext();
    pCtx->InitContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    pCtx->pIO = &io;

    // Setup Dear ImGui style
    if (pCtx->bDarkMode) { ImGui::StyleColorsDark(); }
    else { ImGui::StyleColorsLight(); }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);  
    ImGui_ImplDX11_Init(pCtx->renderContext.pDevice.Get(),
        pCtx->renderContext.pDeviceContext.Get());

    // Main loop
    bool done = false;
    while (!done) {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            ::DispatchMessage(&msg);

            if (msg.message == WM_QUIT) { done = true; }
        }
        if (done) { break; }

        // Handle window resize (we don't resize directly in the WM_SIZE handler)   
        pCtx->renderContext.UpdateResolution(pCtx->resizeWidth, pCtx->resizeHeight);

        // Mainloop
        {
            auto helper = MeasureHelper{ &pCtx->mainLoopTime };
            pCtx->MainLoop();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            auto helper = MeasureHelper{ &pCtx->guiTime };
            gui(pCtx);
        }

        // Rendering
        const static float ColorRGBA[4]
            = { pCtx->clear_color.x * pCtx->clear_color.w,
                pCtx->clear_color.y * pCtx->clear_color.w,
                pCtx->clear_color.z * pCtx->clear_color.w,
                pCtx->clear_color.w };
        pCtx->renderContext.BeginRender(ColorRGBA);
        {
            auto helper = MeasureHelper{ &pCtx->renderTime };
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }
        pCtx->renderContext.EndRender(pCtx->bVSync ? 1 : 0);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    pCtx->UnInitContext();
    pCtx->DestroyContext();
    ImGui::DestroyContext();

    pCtx->renderContext.DestroyContext();
    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// ------------------------------------------------------------
// Helper functions
// ------------------------------------------------------------

// ------------------------------------------
// Window loop
// ------------------------------------------

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
    UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags 
// to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data 
//   to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data 
//   to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from 
// your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    static IMGUIContext* pCtx = nullptr;

    switch (msg) {
    case WM_CREATE:
    {
        const CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);  // NOLINT(performance-no-int-to-ptr)
        pCtx = static_cast<IMGUIContext*>(pCreate->lpCreateParams);

        return 0;
    }
    case WM_SIZE:
    {
        if (wParam == SIZE_MINIMIZED) { return 0; }

        pCtx->resizeWidth = static_cast<UINT>(LOWORD(lParam)); // Queue resize
        pCtx->resizeHeight = static_cast<UINT>(HIWORD(lParam));

        return 0;
    }
    case WM_SYSCOMMAND:
    {
        // Disable ALT application menu
        if ((wParam & 0xfff0) == SC_KEYMENU) { return 0; }

        break;
    }
    case WM_DESTROY:
    {
        ::PostQuitMessage(0);
        return 0;
    }
    default: {
        
    }
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}