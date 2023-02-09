#include "Shader.h"

#include "Helpers.h"

#include <fstream>

Shader::Shader(const std::string& _name, const std::string& vsPath, const std::string& psPath)
    : name(_name), m_VsPath(vsPath), m_PsPath(psPath) {}

bool Shader::Initialize(ID3D11Device* device, HWND hWnd) {
    bool result;
    std::wstring vsCompiledObjPath(m_VsPath.begin(), m_VsPath.end());
    std::wstring psCompiledObjPath(m_PsPath.begin(), m_PsPath.end());

    result = InitializeShader(device, hWnd, vsCompiledObjPath.c_str(), psCompiledObjPath.c_str());
    if (!result) {
        return false;
    }

    return true;
}

void Shader::Shutdown() {
    ShutdownShader();
}

bool Shader::InitializeShader(ID3D11Device* device, HWND hWnd, LPCWSTR vsPath, LPCWSTR psPath) {
    HRESULT result;
    ID3DBlob* errorBlob;
    ID3DBlob* vsBlob;
    ID3DBlob* psBlob;

    errorBlob = nullptr;
    vsBlob = nullptr;
    psBlob = nullptr;

    // Read precompiled objects
    result = D3DReadFileToBlob(vsPath, &vsBlob);
    if (FAILED(result)) {
        MessageBox(nullptr,
            TEXT("Failed to read vertex shader precompiled object to blob."), TEXT("Error"), MB_OK);
        return false;
    }

    result = D3DReadFileToBlob(psPath, &psBlob);
    if (FAILED(result)) {
        MessageBox(nullptr,
            TEXT("Failed to read pixel shader precompiled object to blob."), TEXT("Error"), MB_OK);
        return false;
    }

    // Create shaders
    result = device->CreateVertexShader(vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(), nullptr, &m_VertexShader);
    if (FAILED(result)) {
        return false;
    }

    result = device->CreatePixelShader(psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(), nullptr, &m_PixelShader);
    if (FAILED(result)) {
        return false;
    }

    auto polygonLayout = GenerateInputLayout();

    result = device->CreateInputLayout(polygonLayout.data(), polygonLayout.size(),
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_Layout);
    if (FAILED(result)) {
        return false;
    }

    SafeRelease(vsBlob);
    SafeRelease(psBlob);

    bool ret = CreateConstantBuffers(device);
    if (!ret) {
        return false;
    }

    return true;
}

void Shader::ShutdownShader() {
    ShutdownConstantBuffers();

    if (m_Layout) {
        SafeRelease(m_Layout);
    }

    if (m_PixelShader) {
        SafeRelease(m_PixelShader);
    }

    if (m_VertexShader) {
        SafeRelease(m_VertexShader);
    }
}

void Shader::OutputShaderErrorMessage(ID3DBlob* errorBlob, HWND hWnd, LPCWSTR shaderFilename) {
    char* compileErrors;
    unsigned long long bufferSize;
    std::ofstream fout;

    compileErrors = static_cast<char*>(errorBlob->GetBufferPointer());
    bufferSize = errorBlob->GetBufferSize();

    fout.open("shader_error.txt");
    fout << compileErrors;
    fout.close();

    SafeRelease(errorBlob);

    MessageBox(hWnd,
        TEXT("Failed to compile shader. Error message sent to shader_error.txt."), shaderFilename, MB_OK);
}

void Shader::SetShader(ID3D11DeviceContext* deviceContext) {
    deviceContext->IASetInputLayout(m_Layout);

    deviceContext->VSSetShader(m_VertexShader, nullptr, 0);
    deviceContext->PSSetShader(m_PixelShader, nullptr, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string SimpleShader::GetType() {
    return "normal-to-color";
}

bool SimpleShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, ShaderPayload* payload) {
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    result = deviceContext->Map(m_MatrixBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
        return false;
    }

    dataPtr = static_cast<MatrixBufferType*>(mappedResource.pData);

    dataPtr->world = payload->matrices.world;
    dataPtr->view = payload->matrices.view;
    dataPtr->projection = payload->matrices.projection;

    deviceContext->Unmap(m_MatrixBuffer, 0);

    // TODO: Separate Vertex Shader stage from initialization
    bufferNumber = 0;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

    return true;
}

std::vector<D3D11_INPUT_ELEMENT_DESC> SimpleShader::GenerateInputLayout() {
    std::vector<D3D11_INPUT_ELEMENT_DESC> polygonLayout(3);

    // Create Input Layout
    polygonLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    polygonLayout[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    polygonLayout[2] = { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

    return polygonLayout;
}

bool SimpleShader::CreateConstantBuffers(ID3D11Device* device) {
    HRESULT result;
    D3D11_BUFFER_DESC matrixBufferDesc;

    // Create matrices constant buffer
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_MatrixBuffer);
    if (FAILED(result)) {
        return false;
    }

    return true;
}

void SimpleShader::ShutdownConstantBuffers() {
    if (m_MatrixBuffer) {
        SafeRelease(m_MatrixBuffer);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string PhongShader::GetType() {
    return "phong-lighting";
}

bool PhongShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, ShaderPayload* payload) {
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    LightingMatrices* dataPtr;
    LightProperties* dataPtr2;

    result = deviceContext->Map(m_PerObjectConstantBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
        return false;
    }

    dataPtr = static_cast<LightingMatrices*>(mappedResource.pData);

    dataPtr->worldMatrix                 = payload->lightMatrices.worldMatrix;
    dataPtr->inverseTransposeWorldMatrix = payload->lightMatrices.inverseTransposeWorldMatrix;
    dataPtr->worldViewProjectionMatrix   = payload->lightMatrices.worldViewProjectionMatrix;

    deviceContext->Unmap(m_PerObjectConstantBuffer, 0);

    // TODO: Separate Vertex Shader stage from initialization
    deviceContext->VSSetConstantBuffers(0, 1, &m_PerObjectConstantBuffer);

    result = deviceContext->Map(m_LightPropertiesConstantBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
        return false;
    }

    dataPtr2 = static_cast<LightProperties*>(mappedResource.pData);

    dataPtr2->eyePosition   = payload->lightProperties.eyePosition;
    dataPtr2->globalAmbient = payload->lightProperties.globalAmbient;
    for (int i = 0; i < 8; i++) {
        dataPtr2->lights[i] = payload->lightProperties.lights[i];
    }
    dataPtr2->specularType  = payload->lightProperties.specularType;

    deviceContext->Unmap(m_LightPropertiesConstantBuffer, 0);

    // StartSlot is 1 (check shader register)
    deviceContext->PSSetConstantBuffers(1, 1, &m_LightPropertiesConstantBuffer);

    return true;
}

std::vector<D3D11_INPUT_ELEMENT_DESC> PhongShader::GenerateInputLayout() {
    std::vector<D3D11_INPUT_ELEMENT_DESC> polygonLayout(5);

    // Create Input Layout
    polygonLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    polygonLayout[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    polygonLayout[2] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    polygonLayout[3] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    polygonLayout[4] = { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

    return polygonLayout;
}

bool PhongShader::CreateConstantBuffers(ID3D11Device* device) {
    HRESULT result;
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    // Create matrices constant buffer
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.ByteWidth = sizeof(LightProperties);
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    result = device->CreateBuffer(&constantBufferDesc, nullptr, &m_LightPropertiesConstantBuffer);
    if (FAILED(result)) {
        return false;
    }

    constantBufferDesc.ByteWidth = sizeof(LightingMatrices);
    result = device->CreateBuffer(&constantBufferDesc, nullptr, &m_PerObjectConstantBuffer);
    if (FAILED(result)) {
        return false;
    }

    return true;
}

void PhongShader::ShutdownConstantBuffers() {
    if (m_PerObjectConstantBuffer) {
        SafeRelease(m_PerObjectConstantBuffer);
    }

    if (m_LightPropertiesConstantBuffer) {
        SafeRelease(m_LightPropertiesConstantBuffer);
    }
}