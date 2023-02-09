#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Material.h"
#include "FrustumCulling.h"

class Model {
public:
    bool Initialize(std::string, ID3D11Device*, const char*);
    bool Initialize(std::string, ID3D11Device*, const char*, Material*);
    void InitializeBoundingSphere();
    void Shutdown();

    bool Render(ID3D11DeviceContext*, ShaderPayload*, Matrix) const;
    void SetMaterial(Material*);

private:
    bool ImportModel(const char*);
    void LoadNode(aiNode*, const aiScene*, aiMatrix4x4);
    void LoadMesh(aiMesh*, const aiScene*, aiMatrix4x4, aiMatrix4x4);

public:
    std::string name;
    BoundingSphere boundingSphere;

private:
    std::vector<Mesh> m_Meshes;
    Material* m_Material;

    std::string m_Path;

    friend class Serializer;
    friend class Deserializer;
};

#endif // !_MODEL_H_