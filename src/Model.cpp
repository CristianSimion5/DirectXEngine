#include "Model.hpp"

bool Model::Initialize(ID3D11Device* device, const char* modelPath, DirectX::XMFLOAT3 position) {
    bool result;
    result = ImportModel(modelPath);
    if (!result) {
        return false;
    }
    
    for (Mesh& mesh : m_Meshes) {
        result = mesh.Initialize(device);
        if (!result) {
            return false;
        }
    }
    
    m_Position = position;
    m_ModelMatrix = DirectX::XMMatrixIdentity();
    Translate(position);

    return true;
}

void Model::Shutdown() {
    for (Mesh& mesh : m_Meshes) {
        mesh.Shutdown();
    }
}

bool Model::Render(ID3D11DeviceContext* deviceContext, Shader* shader, 
    DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix) {
    bool renderSuccess = true;
    for (Mesh& mesh : m_Meshes) {
        mesh.Render(deviceContext);

        renderSuccess = shader->Render(deviceContext, mesh.GetIndexCount(),
            { m_ModelMatrix * mesh.GetModelMatrix(), viewMatrix, projectionMatrix});
        if (!renderSuccess) {
            return false;
        }
    }

    return true;
}

void Model::Translate(DirectX::XMFLOAT3 addTo) {
    m_ModelMatrix *= DirectX::XMMatrixTranslation(addTo.x, addTo.y, addTo.z);
}

void Model::Rotate(DirectX::XMFLOAT3 axis, float angle) {
    m_ModelMatrix *= DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&axis), DirectX::XMConvertToRadians(angle));
}

DirectX::XMFLOAT3& Model::GetPosition() {
    return m_Position;
}

DirectX::XMMATRIX& Model::GetModelMatrix() {
    return m_ModelMatrix;
}

bool Model::ImportModel(const char* modelPath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(modelPath, 
        aiProcess_ConvertToLeftHanded | aiProcess_GenSmoothNormals | aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return false;
    }

    LoadNode(scene->mRootNode, scene, aiMatrix4x4());

    return true;
}

void Model::LoadNode(aiNode* node, const aiScene* scene, aiMatrix4x4 transform){
    transform = node->mTransformation * transform;

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        unsigned int index = node->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[index];
        LoadMesh(mesh, scene, transform);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        LoadNode(node->mChildren[i], scene, transform);
    }
}

void Model::LoadMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 transform) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        if (mesh->mTextureCoords[0]) {
            vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        } else {
            vertex.UV = { 0.0f, 0.0f };
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process materials
    
    // Load the transformation into a DirectXMath matrix
    // Although both structures keep matrice in row-major order, 
    // DirectXMath keeps components packed, requiring a transpose
    transform.Transpose();
    DirectX::XMMATRIX dxTransform;
    dxTransform.r[0] = { transform.a1, transform.a2, transform.a3, transform.a4 };
    dxTransform.r[1] = { transform.b1, transform.b2, transform.b3, transform.b4 };
    dxTransform.r[2] = { transform.c1, transform.c2, transform.c3, transform.c4 };
    dxTransform.r[3] = { transform.d1, transform.d2, transform.d3, transform.d4 };
    m_Meshes.push_back(Mesh(vertices, indices, dxTransform));
}
