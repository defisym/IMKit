#include "imguiInterface.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <tchar.h>

// Dear ImGui: standalone example application for DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Forward declarations of helper functions
bool CreateDeviceD3D(GUIContext* pCtx, HWND hWnd);
void CleanupDeviceD3D(GUIContext* pCtx);
void CreateRenderTarget(GUIContext* pCtx);
void CleanupRenderTarget(GUIContext* pCtx);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int ImguiInterface(GUIContext* pCtx,
    const std::function<void(GUIContext*)>& gui) {
    // Create application window
    if (pCtx->bDPIAware) {
        ImGui_ImplWin32_EnableDpiAwareness();
    }
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC,
        WndProc, 0L, 0L,
        GetModuleHandle(nullptr), nullptr,
        nullptr, nullptr, nullptr,
        L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName,
        pCtx->pWindowName,
        WS_OVERLAPPEDWINDOW,
        100, 100,
        pCtx->width, pCtx->height,
        nullptr, nullptr, wc.hInstance, pCtx);

    // Initialize Direct3D
    if (!CreateDeviceD3D(pCtx, hwnd)) {
        CleanupDeviceD3D(pCtx);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    pCtx->CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    pCtx->pIO = &io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);  
    ImGui_ImplDX11_Init(pCtx->renderContext.pD3DDevice, pCtx->renderContext.pD3DDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

	// Main loop
    bool done = false;
    while (!done) {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
       
        if (pCtx->renderContext.resizeWidth != 0 && pCtx->renderContext.resizeHeight != 0) {
            CleanupRenderTarget(pCtx);            
            pCtx->renderContext.pSwapChain->ResizeBuffers(0, 
                pCtx->renderContext.resizeWidth, pCtx->renderContext.resizeHeight,
                DXGI_FORMAT_UNKNOWN, 0);
            pCtx->renderContext.resizeWidth = pCtx->renderContext.resizeHeight = 0;
            CreateRenderTarget(pCtx);
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        gui(pCtx);

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4]
    	= { pCtx->clear_color.x * pCtx->clear_color.w,
            pCtx->clear_color.y * pCtx->clear_color.w,
            pCtx->clear_color.z * pCtx->clear_color.w,
            pCtx->clear_color.w };
        pCtx->renderContext.pD3DDeviceContext->OMSetRenderTargets(1, &pCtx->renderContext.pRenderTargetView, nullptr);
        pCtx->renderContext.pD3DDeviceContext->ClearRenderTargetView(pCtx->renderContext.pRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        pCtx->renderContext.pSwapChain->Present(1, 0); // Present with vsync
        //pCtx->renderContext.pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    pCtx->DestroyContext();
    ImGui::DestroyContext();

    CleanupDeviceD3D(pCtx);
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(GUIContext* pCtx, HWND hWnd) {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    constexpr D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res =
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, createDeviceFlags,
        featureLevelArray, 2,
        D3D11_SDK_VERSION, &sd,
        &pCtx->renderContext.pSwapChain, &pCtx->renderContext.pD3DDevice,
        &featureLevel, &pCtx->renderContext.pD3DDeviceContext);

    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = 
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP,
        nullptr, createDeviceFlags, 
        featureLevelArray, 2, 
        D3D11_SDK_VERSION, &sd, 
        &pCtx->renderContext.pSwapChain, &pCtx->renderContext.pD3DDevice,
        &featureLevel, &pCtx->renderContext.pD3DDeviceContext);

    if (res != S_OK)
        return false;

    CreateRenderTarget(pCtx);
    return true;
}

void CleanupDeviceD3D(GUIContext* pCtx) {
    CleanupRenderTarget(pCtx);

    if (pCtx->renderContext.pSwapChain) {
        pCtx->renderContext.pSwapChain->Release();
        pCtx->renderContext.pSwapChain = nullptr;
    }
    if (pCtx->renderContext.pD3DDeviceContext) {
        pCtx->renderContext.pD3DDeviceContext->Release();
        pCtx->renderContext.pD3DDeviceContext = nullptr;
    }
    if (pCtx->renderContext.pD3DDevice) {
        pCtx->renderContext.pD3DDevice->Release();
        pCtx->renderContext.pD3DDevice = nullptr;
    }
}

void CreateRenderTarget(GUIContext* pCtx) {
    ID3D11Texture2D* pBackBuffer;    
    pCtx->renderContext.pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    pCtx->renderContext.pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pCtx->renderContext.pRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget(GUIContext* pCtx) {
    if (pCtx->renderContext.pRenderTargetView) {
        pCtx->renderContext.pRenderTargetView->Release();
        pCtx->renderContext.pRenderTargetView = nullptr;
    }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    static GUIContext* pCtx = nullptr;

    switch (msg) {
    case WM_CREATE:
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pCtx = static_cast<GUIContext*>(pCreate->lpCreateParams);

        return 0;
    }
    case WM_SIZE:
    {
        if (wParam == SIZE_MINIMIZED) { return 0; }

        pCtx->renderContext.resizeWidth = static_cast<UINT>(LOWORD(lParam)); // Queue resize
        pCtx->renderContext.resizeHeight = static_cast<UINT>(HIWORD(lParam));

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