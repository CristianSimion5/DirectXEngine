#include "Mesh.h"

#include "Helpers.h"

Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> inds, DirectX::XMMATRIX modelMatrix) {
    m_Vertices = verts;
    m_Indices = inds;
    m_ModelMatrix = DirectX::XMMatrixIdentity(); //modelMatrix;
}

bool Mesh::Initialize(ID3D11Device* device) {
    bool result;

    result = InitializeBuffers(device);
    if (!result) {
        return false;
    }

    return true;
}

void Mesh::Shutdown() {
    ShutdownBuffers();
}

void Mesh::Render(ID3D11DeviceContext* deviceContext) {
    RenderBuffers(deviceContext);
}

int Mesh::GetIndexCount() {
    return static_cast<int>(m_Indices.size());
}

DirectX::XMMATRIX& Mesh::GetModelMatrix() {
    return m_ModelMatrix;
}

bool Mesh::InitializeBuffers(ID3D11Device* device) {
    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData;
    D3D11_SUBRESOURCE_DATA indexData;
    HRESULT result;

    // Hardcoded mesh - simple triangle
    /*Vertex* vertices;
    unsigned long* indices;
    m_VertexCount = 3;
    m_IndexCount = 3;

    vertices = new Vertex[m_VertexCount];
    if (!vertices) {
        return false;
    }

    indices = new unsigned long[m_IndexCount];
    if (!indices) {
        return false;
    }

    // Note: Vertex struct no longer has a Color component
    vertices[0].Position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);
    vertices[0].Color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[1].Position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[1].Color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].Position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f);
    vertices[2].Color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    
    // Delete after creating the buffers (moved here to be part of the comment)
    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    */

    // Vertex buffer creation
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_Vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    vertexData.pSysMem = m_Vertices.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
    if (FAILED(result)) {
        return false;
    }

    // Index buffer creation
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_Indices.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    indexData.pSysMem = m_Indices.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
    if (FAILED(result)) {
        return false;
    }
    
    return true;
}

void Mesh::ShutdownBuffers() {
    if (m_IndexBuffer) {
        SafeRelease(m_IndexBuffer);
    }

    if (m_VertexBuffer) {
        SafeRelease(m_VertexBuffer);
    }
}

void Mesh::RenderBuffers(ID3D11DeviceContext* deviceContext) {
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(Vertex);
    offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, 
        &stride, &offset);
    deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
