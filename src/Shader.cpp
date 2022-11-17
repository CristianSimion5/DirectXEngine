#include "Shader.h"

#include "Helpers.h"

#include <fstream>

bool Shader::Initialize(ID3D11Device* device, HWND hWnd) {
    bool result;
    LPCWSTR vsCompiledObjPath;
    LPCWSTR psCompiledObjPath;

#if _DEBUG
    vsCompiledObjPath = L"bin/Debug/VertexShader.cso";
    psCompiledObjPath = L"bin/Debug/PixelShader.cso";
#else
    vsCompiledObjPath = L"bin/Release/VertexShader.cso";
    psCompiledObjPath = L"bin/Release/PixelShader.cso";
#endif

    result = InitializeShader(device, hWnd, vsCompiledObjPath, psCompiledObjPath);
    if (!result) {
        return false;
    }

    return true;
}

void Shader::Shutdown() {
    ShutdownShader();
}

bool Shader::Render(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType matrices) {
    bool result;

    result = SetShaderParameters(deviceContext, matrices);
    if (!result) {
        return false;
    }

    RenderShader(deviceContext, indexCount);

    return true;
}

bool Shader::InitializeShader(ID3D11Device* device, HWND hWnd, LPCWSTR vsPath, LPCWSTR psPath) {
    HRESULT result;
    ID3DBlob* errorBlob;
    ID3DBlob* vsBlob;
    ID3DBlob* psBlob;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

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

    // Create Input Layout
    polygonLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    polygonLayout[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    polygonLayout[2] = { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    //polygonLayout[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

    numElements = _countof(polygonLayout);

    result = device->CreateInputLayout(polygonLayout, numElements,
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_Layout);
    if (FAILED(result)) {
        return false;
    }

    SafeRelease(vsBlob);
    SafeRelease(psBlob);

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

void Shader::ShutdownShader() {
    if (m_MatrixBuffer) {
        SafeRelease(m_MatrixBuffer);
    }

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

bool Shader::SetShaderParameters(ID3D11DeviceContext* deviceContext, MatrixBufferType matrices) {
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

    dataPtr->world = matrices.world;
    dataPtr->view = matrices.view;
    dataPtr->projection = matrices.projection;

    deviceContext->Unmap(m_MatrixBuffer, 0);

    // TODO: Separate Vertex Shader stage from initialization
    bufferNumber = 0;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

    return true;
}

void Shader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
    deviceContext->IASetInputLayout(m_Layout);

    deviceContext->VSSetShader(m_VertexShader, nullptr, 0);
    deviceContext->PSSetShader(m_PixelShader, nullptr, 0);

    deviceContext->DrawIndexed(indexCount, 0, 0);
}
