#include "D3DContext.h"

#include <cmath>
#include <xutility>

// ------------------------------------------------
// D3DContext
// ------------------------------------------------

HRESULT D3DContext::CreateContext(const AdapterSelector& selector) {
    HRESULT hr = S_OK;

    hr = DestroyContext();
    if (FAILED(hr)) { return hr; }

    hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
    if (FAILED(hr)) { return hr; }

    ComPtr<IDXGIAdapter> pAdapter = nullptr;

    if (selector != nullptr) {
        const auto adapterTypes = GetAdapterTypes(pFactory);
        
        if (!adapterTypes.empty()) {
            pAdapter = selector(adapterTypes);
        }
    }

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL featureLevel;
    constexpr D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    hr = D3D11CreateDevice(pAdapter.Get(), 
        pAdapter == nullptr ? D3D_DRIVER_TYPE_HARDWARE : D3D_DRIVER_TYPE_UNKNOWN,
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

D3DContext::AdapterTypes D3DContext::GetAdapterTypes(ComPtr<IDXGIFactory2> pEnumFactory) const {
    if (pEnumFactory == nullptr) {
        HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&pEnumFactory));
        if (FAILED(hr)) { return {}; }
    }

    AdapterTypes adapterTypes = {};

    ComPtr<IDXGIAdapter> pCurAdapter = nullptr;
    for (UINT i = 0; pEnumFactory->EnumAdapters(i, &pCurAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC desc;
        if (FAILED(pCurAdapter->GetDesc(&desc))) { continue; }

        adapterTypes.emplace_back(pCurAdapter, desc);
    }

    return adapterTypes;
}
