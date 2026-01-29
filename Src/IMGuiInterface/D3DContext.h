#pragma once

#define NOMINMAX
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <dxgi1_3.h>
#pragma comment(lib, "dxgi.lib")

#include <vector>

#include "D3DUtilities/D3DDefinition.h"

struct D3DContext {
    ComPtr<IDXGIFactory2> pFactory = nullptr;
    ComPtr<ID3D11Device> pDevice = nullptr;
    ComPtr<ID3D11DeviceContext> pDeviceContext = nullptr;

    HRESULT CreateContext();
    HRESULT DestroyContext();
};
