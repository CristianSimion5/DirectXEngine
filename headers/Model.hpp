#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Material.h"

class Model {
public:
    bool Initialize(ID3D11Device*, const char*);
    bool Initialize(ID3D11Device*, const char*, Material*);
    void Shutdown();

    bool Render(ID3D11DeviceContext*, ShaderPayload*, Matrix) const;
    void SetMaterial(Material*);

private:
    bool ImportModel(const char*);
    void LoadNode(aiNode*, const aiScene*, aiMatrix4x4);
    void LoadMesh(aiMesh*, const aiScene*, aiMatrix4x4, aiMatrix4x4);

private:
    std::vector<Mesh> m_Meshes;
    Material* m_Material;
};

#endif // !_MODEL_H_