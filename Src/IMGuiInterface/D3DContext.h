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

    HRESULT CreateContext();
    HRESULT DestroyContext();
};

struct D3DRenderer {    
    D3DContext* pCtx = nullptr;

    UINT width = 0;
    UINT height = 0;

    virtual HRESULT Init(D3DContext* p);
    virtual HRESULT Destroy();

    virtual HRESULT CreateRenderTarget(UINT width, UINT height) = 0;
    virtual HRESULT DestroyRenderTarget() = 0;
    virtual ComPtr<ID3D11RenderTargetView> GetRenderTargetView() = 0;

    bool ResolutionChanged(UINT width, UINT height) const;
    virtual HRESULT UpdateResolution(UINT width, UINT height);

    virtual void BeginRender();
    virtual void BeginRender(const FLOAT ColorRGBA[4]);
    virtual void EndRender() {}
};

struct D3DRendererSwapChain : D3DRenderer {
    ComPtr<IDXGISwapChain1> pSwapChain = nullptr;
    ComPtr<ID3D11RenderTargetView> pRenderTargetView = nullptr;

    HRESULT Init(D3DContext* p, HWND hWnd);
    HRESULT Destroy() override;

    HRESULT CreateRenderTarget(UINT width, UINT height) override;
    HRESULT DestroyRenderTarget() override;
    ComPtr<ID3D11RenderTargetView> GetRenderTargetView() override;

    HRESULT UpdateResolution(UINT width, UINT height) override;
    
    void EndRender(UINT SyncInterval);
};

struct D3DRendererTexture :D3DRenderer {
	DXGI_FORMAT textureFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

    ComPtr<ID3D11Texture2D> pRTT = nullptr;
    ComPtr<ID3D11ShaderResourceView> pSrvRTT = nullptr;    
    ComPtr<ID3D11RenderTargetView> pRenderTargetView = nullptr;

    HRESULT Init(D3DContext* p,
        DXGI_FORMAT fmt = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
    HRESULT Destroy() override;

    HRESULT CreateRenderTarget(UINT width, UINT height) override;
    HRESULT DestroyRenderTarget() override;
    ComPtr<ID3D11RenderTargetView> GetRenderTargetView() override;

    HRESULT UpdateResolution(UINT width, UINT height) override;

    void EndRender(UINT IndexCount);
};