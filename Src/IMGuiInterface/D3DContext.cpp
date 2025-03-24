#include "D3DContext.h"

#include <xutility>

// ------------------------------------------------
// D3DContext
// ------------------------------------------------

HRESULT D3DContext::CreateContext() {
    HRESULT hr = S_OK;

    hr = DestroyContext();
    if (FAILED(hr)) { return hr; }

    hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
    if (FAILED(hr)) { return hr; }

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL featureLevel;
    constexpr D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, createDeviceFlags,
        featureLevelArray, std::size(featureLevelArray),
        D3D11_SDK_VERSION,
        &pDevice, &featureLevel, &pDeviceContext);

    // Try high-performance WARP software driver if hardware is not available.
    if (hr == DXGI_ERROR_UNSUPPORTED) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP,
            nullptr, createDeviceFlags,
            featureLevelArray, std::size(featureLevelArray),
            D3D11_SDK_VERSION,
            &pDevice, &featureLevel, &pDeviceContext);
    }

    return hr;
}

HRESULT D3DContext::DestroyContext() {
    pFactory = nullptr;
    pDevice = nullptr;
    pDeviceContext = nullptr;

    return S_OK;
}

HRESULT D3DContext::DestroyRenderTarget() {
    pRenderTargetView = nullptr;
    return S_OK;
}

bool D3DContext::ResolutionChanged(UINT width, UINT height) {
    if (this->width == width && this->height == height) {
        return false;
    }

    this->width = width;
    this->height = height;

    return true;
}

void D3DContext::BeginRender() {
    ID3D11RenderTargetView* rtvArr[] = { pRenderTargetView.Get() };
    pDeviceContext->OMSetRenderTargets(std::size(rtvArr), rtvArr, nullptr);
}

void D3DContext::BeginRender(const FLOAT ColorRGBA[4]) {
    BeginRender();
    pDeviceContext->ClearRenderTargetView(pRenderTargetView.Get(), ColorRGBA);
}

// ------------------------------------------------
// D3DContextSwapChain
// ------------------------------------------------

HRESULT D3DContextSwapChain::CreateContext(HWND hWnd) {
    HRESULT hr = S_OK;

    hr = DestroyContext();
    if (FAILED(hr)) { return hr; }

    hr = D3DContext::CreateContext();
    if (FAILED(hr)) { return hr; }

    DXGI_SWAP_CHAIN_DESC1 swapChainDescriptor = {};
    swapChainDescriptor.Width = 0;
    swapChainDescriptor.Height = 0;
    swapChainDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDescriptor.SampleDesc.Count = 1;
    swapChainDescriptor.SampleDesc.Quality = 0;
    swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescriptor.BufferCount = 2;
    swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDescriptor.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
    swapChainDescriptor.Flags = {};

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDescriptor = {};
    swapChainFullscreenDescriptor.Windowed = true;
    swapChainFullscreenDescriptor.RefreshRate.Numerator = 60;
    swapChainFullscreenDescriptor.RefreshRate.Denominator = 1;

    return pFactory->CreateSwapChainForHwnd(pDevice.Get(), hWnd,
        &swapChainDescriptor, &swapChainFullscreenDescriptor,
        nullptr, &pSwapChain);
}

HRESULT D3DContextSwapChain::DestroyContext() {
    pSwapChain = nullptr;
    return D3DContext::DestroyContext();
}

HRESULT D3DContextSwapChain::CreateRenderTarget() {
    HRESULT hr = S_OK;

    hr = DestroyRenderTarget();
    if (FAILED(hr)) { return hr; }

    ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr)) { return hr; }

    return pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRenderTargetView);
}

HRESULT D3DContextSwapChain::DestroyRenderTarget() {
    return D3DContext::DestroyRenderTarget();
}

HRESULT D3DContextSwapChain::UpdateResolution(UINT width, UINT height) {
    if (!ResolutionChanged(width, height)) {
        return S_OK;
    }

    HRESULT hr = S_OK;

    hr = pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) { return hr; }

    return CreateRenderTarget();
}

void D3DContextSwapChain::EndRender(UINT SyncInterval) {
    D3DContext::EndRender();
    pSwapChain->Present(SyncInterval, 0);
}
