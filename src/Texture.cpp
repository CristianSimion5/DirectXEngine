#include "Texture.h"

#include <WICTextureLoader.h>
#include "Helpers.h"

Texture::Texture(std::string _name, ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string filePath, HWND hWnd)
    : name(_name), m_Path(filePath) {
    HRESULT result;
    m_TextureView = nullptr;
    m_SamplerState = nullptr;

    std::wstring wPath(filePath.begin(), filePath.end());
    result = DirectX::CreateWICTextureFromFile(device, deviceContext, 
        wPath.c_str(), nullptr, &m_TextureView, 0);
    if (FAILED(result)) {
        MessageBox(hWnd, TEXT("Failed to load texture."), TEXT("Error"), MB_OK);
        return;
    }

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 1.0f;
    samplerDesc.BorderColor[1] = 1.0f;
    samplerDesc.BorderColor[2] = 1.0f;
    samplerDesc.BorderColor[3] = 1.0f;
    samplerDesc.MinLOD = -FLT_MAX;
    samplerDesc.MaxLOD = FLT_MAX;

    result = device->CreateSamplerState(&samplerDesc, &m_SamplerState);
    if (FAILED(result)) {
        MessageBox(hWnd, TEXT("Failed to create texture sampler state."), TEXT("Error"), MB_OK);
        return;
    }
}

Texture::~Texture() {
    SafeRelease(m_SamplerState);
    SafeRelease(m_TextureView);
}

void Texture::SetTexture(ID3D11DeviceContext* deviceContext) const {
    deviceContext->PSSetSamplers(0, 1, &m_SamplerState);
    deviceContext->PSSetShaderResources(0, 1, &m_TextureView);
}
