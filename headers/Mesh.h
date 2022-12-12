#ifndef _MESH_H_
#define _MESH_H_

#include <d3d11.h>
#include <Transform.h>

#include <vector>

#include <Shader.h>

struct Vertex {
    Vector3 Position;
    //DirectX::XMFLOAT4 Color;
    Vector3 Normal;
    Vector2 UV;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>, std::vector<unsigned int>, Transform);

    bool Initialize(ID3D11Device*);
    void Shutdown();
    void Render(ID3D11DeviceContext*) const;

    int GetIndexCount() const;

private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*) const;

public:
    Transform transform;

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    // material

    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
};

#endif // !_MESH_H_