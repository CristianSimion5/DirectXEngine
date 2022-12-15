#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>


#include "Scene.h"
#include "Shader.h"

namespace DirectX {
    namespace SimpleMath {
        void to_json(nlohmann::ordered_json& j, const Vector3& val) {
            j = { val.x, val.y, val.z };
        }

        void to_json(nlohmann::ordered_json& j, const Vector4& val) {
            j = { val.x, val.y, val.z, val.w };
        }
    }
}

void to_json(nlohmann::ordered_json& j, const Transform& t) {
    j["position"] = t.position;
    j["rotation"] = t.rotation;
    j["scale"] = t.scale;
}

class Serializer {
    using json = nlohmann::ordered_json;

public:
    void SerializeScene(Scene* scene, std::string outFile);
    json SerializeShader(std::string name, Shader* shader);
    json SerializeTexture(std::string name, Texture* texture);
    json SerializeMaterial(std::string name, Material* material);
    json SerializeModel(std::string name, Model* model);
    json SerializeSceneNode(SceneNode* node);
};

void Serializer::SerializeScene(Scene* scene, std::string outFile) {
    json j;
    j["name"] = scene->name;
    j["main_camera"] = scene->m_MainCamera->name;

    for (const auto& shader : scene->m_Shaders) {
        j["shaders"].push_back(SerializeShader(shader.first, shader.second.get()));
    }

    for (const auto& texture : scene->m_Textures) {
        j["textures"].push_back(SerializeTexture(texture.first, texture.second.get()));
    }

    for (const auto& material : scene->m_Materials) {
        j["materials"].push_back(SerializeMaterial(material.first, material.second.get()));
    }

    for (const auto& model : scene->m_Models) {
        j["models"].push_back(SerializeModel(model.first, model.second.get()));
    }

    for (const auto& node : scene->m_SceneRoot->children) {
        j["nodes"].push_back(SerializeSceneNode(node.get()));
    }

    std::ofstream fout(outFile);
    fout << std::setw(4) << j << std::endl;
}

nlohmann::ordered_json Serializer::SerializeShader(std::string name, Shader* shader) {
    json j;
    j["name"] = name;
    j["vs_path"] = shader->m_VsPath;
    j["ps_path"] = shader->m_PsPath;
    j["type"] = shader->GetType();

    return j;
}

inline nlohmann::ordered_json Serializer::SerializeTexture(std::string name, Texture* texture) {
    json j;
    j["name"] = name;
    j["path"] = texture->m_Path;

    return j;
}

inline nlohmann::ordered_json Serializer::SerializeMaterial(std::string name, Material* material) {
    json j;
    j["name"] = name;
    j["type"] = material->GetType();
    j["shader"] = material->m_Shader->name;
    j["textures"] = json::array();
    for (auto texture : material->GetTextures()) {
        j["textures"].push_back(texture->name);
    }
    if (material->GetType() == "phong-lighting") {
        PhongMaterial* pmat = dynamic_cast<PhongMaterial*>(material);
        j["properties"]["emissive"] = pmat->m_MaterialProperties.emissive;
        j["properties"]["ambient"] = pmat->m_MaterialProperties.ambient;
        j["properties"]["diffuse"] = pmat->m_MaterialProperties.diffuse;
        j["properties"]["specular"] = pmat->m_MaterialProperties.specular;
        j["properties"]["specularStrength"] = pmat->m_MaterialProperties.specularStrength;
    }
    else {
        j["properties"] = nullptr;
    }

    return j;
}

inline nlohmann::ordered_json Serializer::SerializeModel(std::string name, Model* model) {
    json j;
    j["name"] = name;
    j["path"] = model->m_Path;
    j["material"] = model->m_Material->name;

    return j;
}

nlohmann::ordered_json Serializer::SerializeSceneNode(SceneNode* node) {
    json j;
    j["name"] = node->name;
    std::string type = node->GetType();
    j["type"] = type;
    j["transform"] = node->transform;
    /*if (node->m_Parent)
        j["parent"] = node->m_Parent->name;
    else
        j["parent"] = nullptr;*/

    if (node->m_Model)
        j["model"] = node->m_Model->name;
    else
        j["model"] = nullptr;

    if (type == "node")
        j["params"] = nullptr;
    else if (type == "camera")
        j["params"] = {
            { "fov", dynamic_cast<Camera*>(node)->m_FieldOfView }
    };
    else if (type == "light") {
        Light* l = dynamic_cast<Light*>(node);
        j["params"]["color"] = l->m_Color;
        j["params"]["attenuation"] = l->m_AttenuationCoef;
        j["params"]["enabled"] = l->m_Enabled;
    }

    j["children"] = json::array();
    for (const auto& child : node->children) {
        j["children"].push_back(SerializeSceneNode(child.get()));
    }

    return j;
}

#endif // !_SERIALIZER_H_
