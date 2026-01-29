#include "D3DTexture.h"

#include <cmath>

size_t D3DTexture2DArray::CalcArraySize(UINT width) const {
    return (size_t)std::ceil((float)(width) / D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
}

HRESULT D3DTexture2DArray::TextureDimensionValid(UINT width, UINT height) const {    
    if (CalcArraySize(width) > D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) {
        return E_INVALIDARG;
    }

    if (height > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION) {
        return E_INVALIDARG;
    }

    return S_OK;
}

void D3DTexture2DArray::UpdateDimension(UINT width, UINT height) {
    texWidth = (std::min)(width, (UINT)D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
    texHeight = height;
    arraySize = CalcArraySize(width);
    texLastWidth = width - (UINT)(texWidth * (arraySize - 1));
}
