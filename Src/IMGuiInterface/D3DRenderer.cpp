#include "D3DRenderer.h"

#include <cmath>

// ------------------------------------------------
// D3DRenderer
// ------------------------------------------------

HRESULT D3DRenderer::Init(const D3DContext* p) {
    auto hr = Destroy();
    if (FAILED(hr)) { return hr; }

    pCtx = p;

    return S_OK;
}

HRESULT D3DRenderer::Destroy() {
    pCtx = nullptr;

    return S_OK;
}

bool D3DRenderer::ResolutionChanged(UINT width, UINT height) const {
    if (this->width == width && this->height == height) {
        return false;
    }

    return true;
}

HRESULT D3DRenderer::UpdateResolution(UINT width, UINT height) {
    this->width = width;
    this->height = height;

    return S_OK;
}

void D3DRenderer::BeginRender() {
    ID3D11RenderTargetView* rtvArr[] = { GetRenderTargetView().Get() };
    pCtx->pDeviceContext->OMSetRenderTargets(std::size(rtvArr), rtvArr, nullptr);
}

void D3DRenderer::BeginRender(const FLOAT ColorRGBA[4]) {
    BeginRender();
    pCtx->pDeviceContext->ClearRenderTargetView(GetRenderTargetView().Get(), ColorRGBA);
}

// ------------------------------------------------
// D3DRendererSwapChain
// ------------------------------------------------

HRESULT D3DRendererSwapChain::Init(const D3DContext* p, HWND hWnd) {
    HRESULT hr = S_OK;

    hr = Destroy();
    if (FAILED(hr)) { return hr; }

    hr = D3DRenderer::Init(p);
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
    //swapChainDescriptor.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
    swapChainDescriptor.Scaling = DXGI_SCALING::DXGI_SCALING_NONE;
    swapChainDescriptor.Flags = {};

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDescriptor = {};
    swapChainFullscreenDescriptor.Windowed = true;
    swapChainFullscreenDescriptor.RefreshRate.Numerator = 60;
    swapChainFullscreenDescriptor.RefreshRate.Denominator = 1;

    return pCtx->pFactory->CreateSwapChainForHwnd(pCtx->pDevice.Get(), hWnd,
        &swapChainDescriptor, &swapChainFullscreenDescriptor,
        nullptr, &pSwapChain);
}

HRESULT D3DRendererSwapChain::Destroy() {
    pSwapChain = nullptr;

    return D3DRenderer::Destroy();
}

HRESULT D3DRendererSwapChain::CreateRenderTarget(UINT width, UINT height) {
    HRESULT hr = S_OK;

    hr = DestroyRenderTarget();
    if (FAILED(hr)) { return hr; }

    // Update resolution
    hr = D3DRenderer::UpdateResolution(width, height);
    if (FAILED(hr)) { return hr; }

    ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr)) { return hr; }

    return pCtx->pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRtv);
}

HRESULT D3DRendererSwapChain::DestroyRenderTarget() {
    pRtv = nullptr;

    return S_OK;
}

ComPtr<ID3D11RenderTargetView> D3DRendererSwapChain::GetRenderTargetView() {
    return pRtv;
}

HRESULT D3DRendererSwapChain::UpdateResolution(UINT width, UINT height) {
    if (!ResolutionChanged(width, height)) {
        return S_OK;
    }

    HRESULT hr = S_OK;

    // Update resolution
    hr = D3DRenderer::UpdateResolution(width, height);
    if (FAILED(hr)) { return hr; }

    // must destroy here, or swap chain cannot be resized
    // unless all outstanding buffer references have been released.
    hr = DestroyRenderTarget();
    if (FAILED(hr)) { return hr; }

    hr = pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) { return hr; }

    return CreateRenderTarget(width, height);
}

void D3DRendererSwapChain::EndRender(UINT SyncInterval) {
    pSwapChain->Present(SyncInterval, 0);
    D3DRenderer::EndRender();
}

// ------------------------------------------------
// D3DRendererTextureBase
// ------------------------------------------------

HRESULT D3DRendererTextureBase::UpdateResolution(UINT width, UINT height) {
    if (!ResolutionChanged(width, height)) {
        return S_OK;
    }

    HRESULT hr = S_OK;

    // Update resolution
    hr = D3DRenderer::UpdateResolution(width, height);
    if (FAILED(hr)) { return hr; }

    return CreateRenderTarget(width, height);
}

void D3DRendererTextureBase::EndRender(UINT IndexCount) {
    pCtx->pDeviceContext->DrawIndexed(IndexCount, 0, 0);
    D3DRenderer::EndRender();
}

// ------------------------------------------------
// D3DRendererTexture
// ------------------------------------------------

HRESULT D3DRendererTexture::Init(const D3DContext* p, const TextureParam& param) {
    HRESULT hr = S_OK;

    hr = Destroy();
    if (FAILED(hr)) { return hr; }

    hr = D3DRenderer::Init(p);
    if (FAILED(hr)) { return hr; }

    this->param = param;

    return S_OK;
}

HRESULT D3DRendererTexture::Destroy() {
    this->param = {};

    return D3DRenderer::Destroy();
}

HRESULT D3DRendererTexture::CreateTexture(UINT width, UINT height) {
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = param.format;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.SampleDesc = { .Count = 1,.Quality = 0 };
    if (param.bShare) {
        desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
    }

    return pCtx->pDevice->CreateTexture2D(&desc, nullptr, &pTex);
}

HRESULT D3DRendererTexture::CreateRenderTargetView() {
    D3D11_RENDER_TARGET_VIEW_DESC rttDesc = {};
    rttDesc.Format = param.format;
    rttDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rttDesc.Texture2D = { .MipSlice = 0 };

    return pCtx->pDevice->CreateRenderTargetView(pTex.Get(), &rttDesc, &pRtv);
}

HRESULT D3DRendererTexture::CreateShaderResourceView() {
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = param.format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    return pCtx->pDevice->CreateShaderResourceView(pTex.Get(), &srvDesc, &pSrv);
}

HRESULT D3DRendererTexture::CreateRenderTarget(UINT width, UINT height) {
    HRESULT hr = S_OK;

    hr = DestroyRenderTarget();
    if (FAILED(hr)) { return hr; }

    // Update resolution
    hr = D3DRenderer::UpdateResolution(width, height);
    if (FAILED(hr)) { return hr; }

    hr = CreateTexture(width, height);
    if (FAILED(hr)) { return hr; }

    hr = CreateRenderTargetView();
    if (FAILED(hr)) { return hr; }

    hr = CreateShaderResourceView();
    if (FAILED(hr)) { return hr; }  

    return S_OK;
}

HRESULT D3DRendererTexture::DestroyRenderTarget() {
    pTex = nullptr;
    pSrv = nullptr;
    pRtv = nullptr;

    return S_OK;
}

ComPtr<ID3D11RenderTargetView> D3DRendererTexture::GetRenderTargetView() {
    return pRtv;
}

// ------------------------------------------------
// D3DRendererTextureArray
// ------------------------------------------------

HRESULT D3DRendererTextureArray::Init(const D3DContext* p, const TextureParam& param) {
    HRESULT hr = S_OK;

    hr = Destroy();
    if (FAILED(hr)) { return hr; }

    hr = D3DRenderer::Init(p);
    if (FAILED(hr)) { return hr; }

    this->param = param;

    return S_OK;
}

HRESULT D3DRendererTextureArray::Destroy() {
    this->param = {};

    return D3DRenderer::Destroy();
}

HRESULT D3DRendererTextureArray::CreateTexture(UINT width, UINT height) {
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = (UINT)arraySize;
    desc.Format = param.format;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.SampleDesc = { .Count = 1,.Quality = 0 };
    if (param.bShare) {
        desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
    }

    return pCtx->pDevice->CreateTexture2D(&desc, nullptr, &pTex);
}

HRESULT D3DRendererTextureArray::CreateShaderResourceView() {
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = param.format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = (UINT)arraySize;

    return pCtx->pDevice->CreateShaderResourceView(pTex.Get(), &srvDesc, &pSrv);
}

HRESULT D3DRendererTextureArray::CreateRenderTargetView() {
    HRESULT hr = S_OK;

    D3D11_RENDER_TARGET_VIEW_DESC rttDesc = {};
    rttDesc.Format = param.format;
    rttDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    rttDesc.Texture2DArray.MipSlice = 0;
    rttDesc.Texture2DArray.FirstArraySlice = 0;
    rttDesc.Texture2DArray.ArraySize = (UINT)arraySize;

    hr = pCtx->pDevice->CreateRenderTargetView(pTex.Get(), &rttDesc, &pRtvMerged);
    if (FAILED(hr)) { return hr; }

    for (size_t idx = 0; idx < arraySize; idx++) {
        D3D11_RENDER_TARGET_VIEW_DESC rttDesc = {};
        rttDesc.Format = param.format;
        rttDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        rttDesc.Texture2DArray.MipSlice = 0;
        rttDesc.Texture2DArray.FirstArraySlice = (UINT)idx;
        rttDesc.Texture2DArray.ArraySize = (UINT)1;

        pRtvArr.emplace_back(nullptr);
        hr = pCtx->pDevice->CreateRenderTargetView(pTex.Get(), &rttDesc, &pRtvArr.back());
        if (FAILED(hr)) { return hr; }
    }

    // use first slice by default
    UpdateRenderTargetSlice();

    return hr;
}

HRESULT D3DRendererTextureArray::CreateRenderTarget(UINT width, UINT height) {
    HRESULT hr = S_OK;

    hr = TextureDimensionValid(width, height);
    if (FAILED(hr)) { return hr; }

    hr = DestroyRenderTarget();
    if (FAILED(hr)) { return hr; }

    // Update resolution
    hr = D3DRenderer::UpdateResolution(width, height);
    if (FAILED(hr)) { return hr; }

    texWidth = (std::min)(width, (UINT)D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
    texHeight = height;
    arraySize = CalcArraySize(width);
    texLastWidth = width - (UINT)(texWidth * (arraySize - 1));

    hr = CreateTexture(texWidth, texHeight);
    if (FAILED(hr)) { return hr; }

    hr = CreateRenderTargetView();
    if (FAILED(hr)) { return hr; }

    return S_OK;
}

HRESULT D3DRendererTextureArray::DestroyRenderTarget() {
    return S_OK;
}

ComPtr<ID3D11RenderTargetView> D3DRendererTextureArray::GetRenderTargetView() {
    return pRtv;
}
