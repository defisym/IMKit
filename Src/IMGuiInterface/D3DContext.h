#pragma once

#define NOMINMAX
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <dxgi1_3.h>
#pragma comment(lib, "dxgi.lib")

#include "D3DUtilities/D3DDefinition.h"

struct D3DContext {    
    ComPtr<IDXGIFactory2> pFactory = nullptr;
    ComPtr<ID3D11Device> pDevice = nullptr;
    ComPtr<ID3D11DeviceContext> pDeviceContext = nullptr;
    ComPtr<ID3D11RenderTargetView> pRenderTargetView = nullptr;
    
    UINT width = 0;
    UINT height = 0;

    virtual HRESULT CreateContext();
    virtual HRESULT DestroyContext();

    virtual HRESULT CreateRenderTarget(UINT width, UINT height) = 0;
    virtual HRESULT DestroyRenderTarget();

    bool ResolutionChanged(UINT width, UINT height);
    virtual HRESULT UpdateResolution(UINT width, UINT height) = 0;

    virtual void BeginRender();
    virtual void BeginRender(const FLOAT ColorRGBA[4]);
    virtual void EndRender() {}
};

struct D3DContextSwapChain : D3DContext {
    ComPtr<IDXGISwapChain1> pSwapChain = nullptr;

    HRESULT CreateContext(HWND hWnd);
    HRESULT DestroyContext() override;

    HRESULT CreateRenderTarget(UINT width, UINT height) override;
    HRESULT DestroyRenderTarget() override;

    HRESULT UpdateResolution(UINT width, UINT height) override;
    
    void EndRender(UINT SyncInterval);
};

struct D3DContextTexture :D3DContext {
    ComPtr<ID3D11Texture2D> pRTT = nullptr;
    ComPtr<ID3D11ShaderResourceView> pSrvRTT = nullptr;
    ComPtr<ID3D11RenderTargetView> pRTV = nullptr;

    HRESULT CreateRenderTarget(UINT width, UINT height) override;
    HRESULT DestroyRenderTarget() override;

    HRESULT UpdateResolution(UINT width, UINT height) override;

    void EndRender(UINT IndexCount);
};