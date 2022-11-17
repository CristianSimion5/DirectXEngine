#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class Model {
public:
    bool Initialize(ID3D11Device*, const char*, DirectX::XMFLOAT3 = {0.0f, 0.0f, 0.0f});
    void Shutdown();
    bool Render(ID3D11DeviceContext*, Shader*, DirectX::XMMATRIX, DirectX::XMMATRIX);

    void Translate(DirectX::XMFLOAT3);
    void Rotate(DirectX::XMFLOAT3, float);

    DirectX::XMFLOAT3& GetPosition();
    DirectX::XMMATRIX& GetModelMatrix();

private:
    bool ImportModel(const char*);
    void LoadNode(aiNode*, const aiScene*, aiMatrix4x4);
    void LoadMesh(aiMesh*, const aiScene*, aiMatrix4x4);

private:
    std::vector<Mesh> m_Meshes;

    DirectX::XMFLOAT3 m_Position;
    DirectX::XMMATRIX m_ModelMatrix;
};

#endif // !_MODEL_H_