#pragma once

#include "IMGuiInterface/D3DContext.h"

struct IMGUITexture {
    int width = 0;
    int height = 0;

    ComPtr<ID3D11Texture2D> pTexture = nullptr;
    ComPtr<ID3D11ShaderResourceView> pSrv = nullptr;
};

// Simple helper function to load an image into a DX11 texture with common settings
IMGUITexture LoadTextureFromMemory(ID3D11Device* pDevice, const unsigned char* pData, size_t sz);

// Open and read a file, then forward to LoadTextureFromMemory()
IMGUITexture LoadTextureFromFile(ID3D11Device* pDevice, const char* pFileName);