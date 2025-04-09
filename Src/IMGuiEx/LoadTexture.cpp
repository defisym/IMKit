#include "LoadTexture.h"

#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "3rdLib/STB/stb_image.h"

// Simple helper function to load an image into a DX11 texture with common settings
IMGUITexture LoadTextureFromMemory(ID3D11Device* pDevice, const unsigned char* pData, size_t sz) {
    // Load from disk into a raw RGBA buffer
    int width = 0;
    int height = 0;
    unsigned char* pImageData = stbi_load_from_memory(pData, (int)sz,
        &width, &height, nullptr, 4);
    if (pImageData == nullptr) { return {}; }

    // Create texture
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA subResource = {};
    subResource.pSysMem = pImageData;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;

    ComPtr<ID3D11Texture2D> pTexture = nullptr;
    hr = pDevice->CreateTexture2D(&desc, &subResource, &pTexture);
    if (FAILED(hr)) { return {}; }

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    ComPtr<ID3D11ShaderResourceView> pSrv = nullptr;
    hr = pDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pSrv);
    if (FAILED(hr)) { return {}; }
    
    stbi_image_free(pImageData);

    return { .width = width, .height = height,
        .pTexture = pTexture, .pSrv = pSrv };
}

// Open and read a file, then forward to LoadTextureFromMemory()
IMGUITexture LoadTextureFromFile(ID3D11Device* pDevice, const char* pFileName) {
    FILE* fp = nullptr;
    const auto err = fopen_s(&fp, pFileName, "rb");
    if (err != 0 || fp == nullptr) { return {}; }

    fseek(fp, 0, SEEK_END);
    size_t fileSz = (size_t)ftell(fp);
    if (fileSz == -1) { return {}; }
    auto pFileData = std::make_unique<unsigned char[]>(fileSz);

    fseek(fp, 0, SEEK_SET);
    fread(pFileData.get(), 1, fileSz, fp);
    fclose(fp);

    return LoadTextureFromMemory(pDevice, pFileData.get(), fileSz);
}
