#include "Material.h"

#include "Helpers.h"

Material::Material(Shader* shader): m_Shader(shader) {}

void Material::SetShader(ID3D11DeviceContext* deviceContext) {
    m_Shader->SetShader(deviceContext);
}

NormalAsColorMaterial::NormalAsColorMaterial(Shader* shader): Material(shader) {}

bool NormalAsColorMaterial::SetShaderPerMeshData(ID3D11DeviceContext* deviceContext, ShaderPayload* shaderPayload) {
    return m_Shader->SetShaderParameters(deviceContext, shaderPayload);
}

bool NormalAsColorMaterial::SetShaderMaterialParameters(ID3D11DeviceContext* deviceContext, ShaderPayload* shaderPayload) {
    return true;
}


PhongMaterial::PhongMaterial(ID3D11Device* device, Shader* shader, const Texture* texture, PhongMaterialProperties phongMaterial)
    : Material(shader), m_Texture(texture), m_MaterialProperties(phongMaterial) {
    HRESULT result;
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    // Create matrices constant buffer
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.ByteWidth = sizeof(PhongMaterialProperties);
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    result = device->CreateBuffer(&constantBufferDesc, nullptr, &m_MaterialConstantBuffer);
    if (FAILED(result)) {
        m_MaterialConstantBuffer = nullptr;
    }
}

PhongMaterial::~PhongMaterial() {
    if (m_MaterialConstantBuffer) {
        SafeRelease(m_MaterialConstantBuffer);
    }
}

bool PhongMaterial::SetShaderPerMeshData(ID3D11DeviceContext* deviceContext, ShaderPayload* shaderPayload) {
    return m_Shader->SetShaderParameters(deviceContext, shaderPayload);
}

bool PhongMaterial::SetShaderMaterialParameters(ID3D11DeviceContext* deviceContext, ShaderPayload* shaderPayload) {
    // Sets constant buffers, textures, and samplers
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    PhongMaterialProperties* dataPtr;

    result = deviceContext->Map(m_MaterialConstantBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
        return false;
    }

    dataPtr = static_cast<PhongMaterialProperties*>(mappedResource.pData);
    
    dataPtr->emissive         = shaderPayload->phongMaterial.emissive;
    dataPtr->ambient          = shaderPayload->phongMaterial.ambient;
    dataPtr->diffuse          = shaderPayload->phongMaterial.diffuse;
    dataPtr->specular         = shaderPayload->phongMaterial.specular;
    dataPtr->specularStrength = shaderPayload->phongMaterial.specularStrength;

    deviceContext->Unmap(m_MaterialConstantBuffer, 0);

    deviceContext->PSSetConstantBuffers(0, 1, &m_MaterialConstantBuffer);
    m_Texture->SetTexture(deviceContext);

    return true;
}