#include "D3DReadBack.h"

// ------------------------------------------------
// D3DReadBack
// ------------------------------------------------

HRESULT D3DReadBack::Init(const D3DContext* p) {
    pCtx = p;

    return S_OK;
}

void D3DReadBack::CopyResource(ID3D11Resource* pSrc) {
    pCtx->pDeviceContext->CopyResource(GetResource(), pSrc);
    pCtx->pDeviceContext->Flush();
}

// ------------------------------------------------
// D3DReadBackTexture
// ------------------------------------------------

ID3D11Resource* D3DReadBackTexture::GetResource() {
    return pReadBack.Get();
}

HRESULT D3DReadBackTexture::CreateTexture(D3D11_TEXTURE2D_DESC desc) {
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.SampleDesc = { .Count = 1,.Quality = 0 };
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    return pCtx->pDevice->CreateTexture2D(&desc, nullptr, &pReadBack);
}

void D3DReadBackTexture::Map(const UINT subresource, 
    const std::function<void(const D3D11_MAPPED_SUBRESOURCE& mapped)>& cb) {
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    HRESULT hr = pCtx->pDeviceContext->Map(GetResource(), subresource, D3D11_MAP_READ, 0, &mapped);

    if (FAILED(hr)) { return; }
    bMap = true;

    cb(mapped);

    pCtx->pDeviceContext->Unmap(GetResource(), subresource);
    bMap = false;
}

// ------------------------------------------------
// D3DReadBackBuffer
// ------------------------------------------------

ID3D11Resource* D3DReadBackBuffer::GetResource() {
    return pReadBack.Get();
}

HRESULT D3DReadBackBuffer::CreateBuffer(D3D11_BUFFER_DESC desc) {
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    // no D3D11_RESOURCE_MISC_BUFFER_STRUCTURED flag for staging buffer
    desc.MiscFlags = 0;

   return pCtx->pDevice->CreateBuffer(&desc, nullptr, &pReadBack);
}

void D3DReadBackBuffer::Map(const std::function<void(const D3D11_MAPPED_SUBRESOURCE& mapped)>& cb) {
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    HRESULT hr = pCtx->pDeviceContext->Map(GetResource(), 0, D3D11_MAP_READ, 0, &mapped);

    if (FAILED(hr)) { return; }
    bMap = true;

    cb(mapped);

    pCtx->pDeviceContext->Unmap(GetResource(), 0);
    bMap = false;
}
