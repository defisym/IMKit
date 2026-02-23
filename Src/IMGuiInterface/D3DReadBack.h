#pragma once

#include "D3DContext.h"

#include <atomic>
#include <memory>
#include <functional>

struct D3DReadBack {
    const D3DContext* pCtx = nullptr;
    HRESULT Init(const D3DContext* p);

    virtual ID3D11Resource* GetResource() = 0;
    void CopyResource(ID3D11Resource* pSrc);

    std::atomic<bool> bMap = false;
};

struct D3DReadBackTexture :D3DReadBack {
    ComPtr<ID3D11Texture2D> pReadBack = nullptr;

    ID3D11Resource* GetResource() override;
    HRESULT CreateTexture(D3D11_TEXTURE2D_DESC desc);
    void Map(const UINT subresource,
        const std::function<void(const D3D11_MAPPED_SUBRESOURCE& mapped)>& cb);
};

std::unique_ptr<D3DReadBackTexture> GetReadBackTexture(const D3DContext* p, ID3D11Texture2D* pTex);

struct D3DReadBackBuffer :D3DReadBack {
    ComPtr<ID3D11Buffer> pReadBack = nullptr;

    ID3D11Resource* GetResource() override;
    HRESULT CreateBuffer(D3D11_BUFFER_DESC desc);
    void Map(const std::function<void(const D3D11_MAPPED_SUBRESOURCE& mapped)>& cb);
};

std::unique_ptr<D3DReadBackBuffer> GetReadBackBuffer(const D3DContext* p, ID3D11Buffer* pBuf);
