#pragma once

#include "D3DContext.h"

struct SRV {};
struct RTV {};
struct RTVARR {};
struct UAV {};

template<typename T> struct IView {};

template<> struct IView<SRV> {
    ComPtr<ID3D11ShaderResourceView> pSrv;

    virtual HRESULT CreateShaderResourceView() = 0;
};

template<> struct IView<RTV> {
    ComPtr<ID3D11RenderTargetView> pRtv;

    virtual HRESULT CreateRenderTargetView() = 0;
};

template<> struct IView<RTVARR> {
    ComPtr<ID3D11RenderTargetView> pRtv;
    virtual HRESULT CreateRenderTargetView() = 0;

    // merged, for CS/GS
    ComPtr<ID3D11RenderTargetView> pRtvMerged = nullptr;
    void UpdateRenderTargetMerged() {
        pRtv = pRtvMerged;
    }
    // slice, for PS
    std::vector<ComPtr<ID3D11RenderTargetView>> pRtvArr = {};
    void UpdateRenderTargetSlice(size_t index = 0) {
        pRtv = index >= pRtvArr.size()
            ? pRtvArr.front()
            : pRtvArr[index];
    }
};

template<> struct IView<UAV> {
    ComPtr<ID3D11UnorderedAccessView> pUav;

    virtual HRESULT CreateUnorderedAccessView() = 0;
};

template<typename... ViewType>
struct View : IView<ViewType>... {};