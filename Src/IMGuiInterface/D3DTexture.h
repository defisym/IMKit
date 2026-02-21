#pragma once

#include "D3DContext.h"
#include "D3DView.h"

struct TextureParam {
    DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
    bool bShare = true;
};

struct D3DTexture2D {
    TextureParam param = {};
    ComPtr<ID3D11Texture2D> pTex = nullptr;
    
    virtual D3D11_TEXTURE2D_DESC GetDesc(UINT width, UINT height) = 0;
    virtual HRESULT CreateTexture(UINT width, UINT height) = 0;
};

struct D3DTexture2DArray :D3DTexture2D {
    UINT texWidth = 0;      // width of single texture
                            // if not match the width then 16384
    UINT texHeight = 0;     // height of single texture
    size_t arraySize = 0;   // ceil(totalWidth / width)
    UINT texLastWidth = 0;  // width of last texture in array

    size_t CalcArraySize(UINT width) const;
    HRESULT TextureDimensionValid(UINT width, UINT height) const;
    void UpdateDimension(UINT width, UINT height);
};
