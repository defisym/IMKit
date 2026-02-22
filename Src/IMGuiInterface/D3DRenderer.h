#pragma once

#include "D3DContext.h"
#include "D3DTexture.h"

struct D3DRenderer {
    const D3DContext* pCtx = nullptr;

    UINT width = 0;
    UINT height = 0;

    virtual HRESULT Init(const D3DContext* p);
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
    ComPtr<ID3D11RenderTargetView> pRtv = nullptr;

    HRESULT Init(const D3DContext* p, HWND hWnd);
    HRESULT Destroy() override;

    HRESULT CreateRenderTarget(UINT width, UINT height) override;
    HRESULT DestroyRenderTarget() override;
    ComPtr<ID3D11RenderTargetView> GetRenderTargetView() override;

    HRESULT UpdateResolution(UINT width, UINT height) override;

    void EndRender(UINT SyncInterval);
};

struct D3DRendererTextureBase :D3DRenderer {
    virtual HRESULT Init(const D3DContext* p, const TextureParam& param = {}) = 0;
    virtual HRESULT Destroy() override = 0;

    HRESULT UpdateResolution(UINT width, UINT height) override;

    void EndRender(UINT IndexCount);
};

struct D3DRendererTexture :D3DRendererTextureBase, D3DTexture2D, View<SRV, RTV> {
    HRESULT Init(const D3DContext* p, const TextureParam& param = {});
    HRESULT Destroy() override;

    D3D11_TEXTURE2D_DESC GetDesc(UINT width, UINT height) const override;
    HRESULT CreateTexture(UINT width, UINT height) override;
    HRESULT CreateRenderTargetView() override;
    HRESULT CreateShaderResourceView() override;

    HRESULT CreateRenderTarget(UINT width, UINT height) override;
    HRESULT DestroyRenderTarget() override;
    ComPtr<ID3D11RenderTargetView> GetRenderTargetView() override;
};

struct D3DRendererTextureArray :D3DRendererTextureBase, D3DTexture2DArray, View<SRV, RTVARR> {
    HRESULT Init(const D3DContext* p, const TextureParam& param = {});
    HRESULT Destroy() override;

    D3D11_TEXTURE2D_DESC GetDesc(UINT width, UINT height) const override;
    HRESULT CreateTexture(UINT width, UINT height) override;
    HRESULT CreateShaderResourceView() override;
    HRESULT CreateRenderTargetView() override;

    HRESULT CreateRenderTarget(UINT width, UINT height) override;
    HRESULT DestroyRenderTarget() override;
    ComPtr<ID3D11RenderTargetView> GetRenderTargetView() override;
};