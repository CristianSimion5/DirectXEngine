#include "Model.hpp"

bool Model::Initialize(std::string _name, ID3D11Device* device, const char* modelPath) {
    bool result;
    name = _name;
    m_Path = modelPath;
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

    InitializeBoundingSphere();

    return true;
}

bool Model::Initialize(std::string _name, ID3D11Device* device, const char* modelPath, Material* material) {
    bool result = Initialize(_name, device, modelPath);
    if (!result) {
        return false;
    }

    m_Material = material;

    return true;
}

void Model::InitializeBoundingSphere() {
    Vector3 minCoords(1e9f), maxCoords(-1e9f);
    for (const Mesh& mesh : m_Meshes) {
        for (const Vertex& vertex : mesh.GetVertices()) {
            minCoords = Vector3::Min(minCoords, vertex.Position);
            maxCoords = Vector3::Max(maxCoords, vertex.Position);
        }
    }
    //float radius = Vector3::Distance(maxCoords, minCoords) * 0.5f;
    Vector3 center = (minCoords + maxCoords) * 0.5f;
    float radius = 0.0f;
    for (const Mesh& mesh : m_Meshes) {
        for (const Vertex& vertex : mesh.GetVertices()) {
            radius = std::max(radius, Vector3::Distance(center, vertex.Position));
        }
    }
    boundingSphere = BoundingSphere(center, radius);
}

void Model::Shutdown() {
    for (Mesh& mesh : m_Meshes) {
        mesh.Shutdown();
    }
}

bool Model::Render(ID3D11DeviceContext* deviceContext, ShaderPayload* shaderPayload, Matrix worldMatrix) const {
    if (!m_Material) {
        return true;
    }

    bool renderSuccess = true;
    // Add per material constant buffer data
    m_Material->SetShader(deviceContext);
    renderSuccess = m_Material->SetShaderMaterialParameters(deviceContext, shaderPayload);
    if (!renderSuccess) {
        return false;
    }

    for (const Mesh& mesh : m_Meshes) {
        // Add per mesh constant buffer data
        shaderPayload->matrices.world = mesh.transform.globalMatrix * worldMatrix;
        shaderPayload->lightMatrices.worldMatrix = mesh.transform.globalMatrix * worldMatrix;
        shaderPayload->lightMatrices.worldViewProjectionMatrix = shaderPayload->lightMatrices.worldMatrix 
           * shaderPayload->viewProjectionMatrix;
        shaderPayload->lightMatrices.inverseTransposeWorldMatrix =
            shaderPayload->lightMatrices.worldMatrix.Invert().Transpose();

        mesh.BindBuffers(deviceContext);
        renderSuccess = m_Material->SetShaderPerMeshData(deviceContext, shaderPayload);
            //{ m_ModelMatrix * mesh.GetModelMatrix(), viewMatrix, projectionMatrix});
            //{ mesh.transform.globalMatrix * worldMatrix, viewMatrix, projectionMatrix});
        if (!renderSuccess) {
            return false;
        }

        deviceContext->DrawIndexed(mesh.GetIndexCount(), 0, 0);
    }

    return true;
}

void Model::SetMaterial(Material* material) {
    m_Material = material;
}

bool Model::ImportModel(const char* modelPath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(modelPath, 
        aiProcess_ConvertToLeftHanded | aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return false;
    }

    /*aiMatrix4x4 aiInitialMatrix(
        initialMatrix.m[0][0], initialMatrix.m[1][0], initialMatrix.m[2][0], initialMatrix.m[3][0],
        initialMatrix.m[0][1], initialMatrix.m[1][1], initialMatrix.m[2][1], initialMatrix.m[3][1],
        initialMatrix.m[0][2], initialMatrix.m[1][2], initialMatrix.m[2][2], initialMatrix.m[3][2],
        initialMatrix.m[0][3], initialMatrix.m[1][3], initialMatrix.m[2][3], initialMatrix.m[3][3]
    );*/
    LoadNode(scene->mRootNode, scene, aiMatrix4x4());

    return true;
}

void Model::LoadNode(aiNode* node, const aiScene* scene, aiMatrix4x4 transform){
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        unsigned int index = node->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[index];
        LoadMesh(mesh, scene, transform, node->mTransformation);
    }

    transform = node->mTransformation * transform;
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        LoadNode(node->mChildren[i], scene, transform);
    }
}

void Model::LoadMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 parentTransform, aiMatrix4x4 localTransform) {
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
        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
            vertex.Bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
        } else {
            vertex.Tangent = { 0.0f, 0.0f, 0.0f };
            vertex.Bitangent = { 0.0f, 0.0f, 0.0f };
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
    // Although both structures keep matrices in row-major order, 
    // DirectXMath keeps components packed by using row-major mathematical notation,
    // while ASSIMP uses column-major mathematical notation, requiring a transpose
    parentTransform.Transpose();
    Matrix pMatrix(
        parentTransform.a1, parentTransform.a2, parentTransform.a3, parentTransform.a4,
        parentTransform.b1, parentTransform.b2, parentTransform.b3, parentTransform.b4,
        parentTransform.c1, parentTransform.c2, parentTransform.c3, parentTransform.c4,
        parentTransform.d1, parentTransform.d2, parentTransform.d3, parentTransform.d4
    );
    /*dxTransform.r[0] = { transform.a1, transform.a2, transform.a3, transform.a4 };
    dxTransform.r[1] = { transform.b1, transform.b2, transform.b3, transform.b4 };
    dxTransform.r[2] = { transform.c1, transform.c2, transform.c3, transform.c4 };
    dxTransform.r[3] = { transform.d1, transform.d2, transform.d3, transform.d4 };*/

    Matrix lMatrix(
        localTransform.a1, localTransform.a2, localTransform.a3, localTransform.a4,
        localTransform.b1, localTransform.b2, localTransform.b3, localTransform.b4,
        localTransform.c1, localTransform.c2, localTransform.c3, localTransform.c4,
        localTransform.d1, localTransform.d2, localTransform.d3, localTransform.d4
    );
    m_Meshes.push_back(Mesh(vertices, indices, Transform(pMatrix, lMatrix)));
}