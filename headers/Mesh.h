#ifndef _MESH_H_
#define _MESH_H_

#include <d3d11.h>
#include <DirectXMath.h>

#include <vector>

#include <Shader.h>

struct Vertex {
    DirectX::XMFLOAT3 Position;
    //DirectX::XMFLOAT4 Color;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT2 UV;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>, std::vector<unsigned int>, DirectX::XMMATRIX);

    bool Initialize(ID3D11Device*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    int GetIndexCount();
    DirectX::XMMATRIX& GetModelMatrix();

private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    // material

    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
    DirectX::XMMATRIX m_ModelMatrix;
};

#endif // !_MESH_H_