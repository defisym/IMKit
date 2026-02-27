#pragma once

#define NOMINMAX
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <dxgi1_3.h>
#pragma comment(lib, "dxgi.lib")

#include <vector>
#include <functional>

#include "D3DUtilities/D3DDefinition.h"

struct D3DContext {
    ComPtr<IDXGIFactory2> pFactory = nullptr;
    ComPtr<ID3D11Device> pDevice = nullptr;
    ComPtr<ID3D11DeviceContext> pDeviceContext = nullptr;

    struct AdapterInfo {
        ComPtr<IDXGIAdapter> pAdapter = nullptr;
        DXGI_ADAPTER_DESC desc = {};
    };

    using AdapterTypes = std::vector<AdapterInfo>;
    using AdapterSelector = std::function<ComPtr<IDXGIAdapter>(const AdapterTypes&)>;

    HRESULT CreateContext(const AdapterSelector& selector = nullptr);
    HRESULT DestroyContext();

    AdapterTypes GetAdapterTypes() const;
};
