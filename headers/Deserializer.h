#ifndef _DESERIALIZER_H_
#define _DESERIALIZER_H_

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Scene.h"
#include "Shader.h"

namespace DirectX {
    namespace SimpleMath {
        void from_json(const nlohmann::json& j, Vector3& val) {
            val = Vector3(j[0], j[1], j[2]);
        }

        void from_json(const nlohmann::json& j, Vector4& val) {
            val = Vector4(j[0], j[1], j[2], j[3]);
        }
    }
}

void from_json(const nlohmann::json& j, Transform& t) {
    t.position = j["position"].get<Vector3>();
    t.rotation = j["rotation"].get<Vector3>();
    t.scale = j["scale"].get<Vector3>();
}

class Deserializer {
    using json = nlohmann::json;

public:
    void DeserializeScene(Scene* scene, std::string inFile);
    void DeserializeShader(const json& j, Scene* scene);
    void DeserializeTexture(const json& j, Scene* scene);
    void DeserializeMaterial(const json& j, Scene* scene);
    void DeserializeModel(const json& j, Scene* scene);
    void DeserializeSceneNode(SceneNode* parentNode, const json& j, Scene* scene);
};

void Deserializer::DeserializeScene(Scene* scene, std::string inFile) {
    std::ifstream fin(inFile);
    json j = json::parse(fin);

    scene->name = j["name"];

    for (const auto& jShader : j["shaders"]) {
        DeserializeShader(jShader, scene);
    }

    for (const auto& jTexture : j["textures"]) {
        DeserializeTexture(jTexture, scene);
    }

    for (const auto& jMaterial : j["materials"]) {
        DeserializeMaterial(jMaterial, scene);
    }

    for (const auto& jModel : j["models"]) {
        DeserializeModel(jModel, scene);
    }

    scene->m_SceneRoot = std::make_unique<SceneNode>("root");
    for (const auto& jNode : j["nodes"]) {
        DeserializeSceneNode(scene->m_SceneRoot.get(), jNode, scene);
    }

    //j["main_camera"] = scene->m_MainCamera->name;
}

void Deserializer::DeserializeShader(const json& j, Scene* scene) {
    std::unique_ptr<Shader> shader;

    if (j["type"] == "normal-to-color") {
        shader = std::make_unique<SimpleShader>(j["name"], j["vs_path"], j["ps_path"]);
    }
    else if (j["type"] == "phong-lighting") {
        shader = std::make_unique<PhongShader>(j["name"], j["vs_path"], j["ps_path"]);
    }
    shader->Initialize(scene->m_Device, scene->m_hWnd);

    scene->m_Shaders.insert({ shader->name, std::move(shader) });
}

inline void Deserializer::DeserializeTexture(const json& j, Scene* scene) {
    std::unique_ptr<Texture> texture = std::make_unique<Texture>(
        j["name"], 
        scene->m_Device, scene->m_DeviceContext, 
        j["path"], 
        scene->m_hWnd);

    scene->m_Textures.insert({ texture->name, std::move(texture) });
}

inline void Deserializer::DeserializeMaterial(const json& j, Scene* scene) {
    std::unique_ptr<Material> material;
    if (j["type"] == "normal-to-color") {
        material = std::make_unique<NormalAsColorMaterial>(
            j["name"],
            scene->m_Shaders[j["shader"]].get()
            );
    }
    else if (j["type"] == "phong-lighting") {
        PhongMaterialProperties properties;
        properties.emissive = j["properties"]["emissive"];
        properties.ambient = j["properties"]["ambient"];
        properties.diffuse = j["properties"]["diffuse"];
        properties.specular = j["properties"]["specular"];
        properties.specularStrength = j["properties"]["specularStrength"];

        material = std::make_unique<PhongMaterial>(
            j["name"],
            scene->m_Device,
            scene->m_Shaders[j["shader"]].get(),
            scene->m_Textures[j["textures"][0]].get(),
            properties
            );
        if (j["textures"].size() >= 2) {
            PhongMaterial* tempMat = dynamic_cast<PhongMaterial*>(material.get());
            tempMat->SetNormalMap(scene->m_Textures[j["textures"][1]].get());
        }
        if (j["textures"].size() >= 3) {
            PhongMaterial* tempMat = dynamic_cast<PhongMaterial*>(material.get());
            tempMat->SetHeightMap(scene->m_Textures[j["textures"][2]].get());
        }
    }
    
    scene->m_Materials.insert({ material->name, std::move(material) });
}

inline void Deserializer::DeserializeModel(const json& j, Scene* scene) {
    std::unique_ptr<Model> model = std::make_unique<Model>();

    model->Initialize(j["name"].get<std::string>(), 
        scene->m_Device, 
        j["path"].get<std::string>().c_str(),
        scene->m_Materials[j["material"].get<std::string>()].get());

    scene->m_Models.insert({ model->name, std::move(model) });
}

void Deserializer::DeserializeSceneNode(SceneNode* parentNode, const json& j, Scene* scene) {
    std::unique_ptr<SceneNode> node;
    
    Model* nodeModel = nullptr;
    if (j["model"] != nullptr) {
        nodeModel = scene->m_Models[j["model"]].get();
    }
    if (j["type"] == "node") {
        node = std::make_unique<SceneNode>(
            j["name"],
            parentNode,
            nodeModel
            );
    }
    else if (j["type"] == "camera") {
        // TODO: handle camera deserialization more gracefully
        // right now main camera logic considers there's only one camera in the scene 
        // (or the last one found in the hierarchy)
        node = std::make_unique<Camera>(
            j["name"],
            parentNode,
            nodeModel
            );
        scene->m_MainCamera = dynamic_cast<Camera*>(node.get());
    }
    else if (j["type"] == "light") {
        node = std::make_unique<Light>(
            j["name"],
            j["params"]["color"].get<Vector4>(),
            j["params"]["attenuation"],
            j["params"]["enabled"],
            parentNode,
            nodeModel
            );
        scene->m_Lights.insert({ node->name, dynamic_cast<Light*>(node.get()) });
    }
    node->transform = j["transform"];

    for (const auto& jNode : j["children"]) {
        DeserializeSceneNode(node.get(), jNode, scene);
    }

    parentNode->AddChild(std::move(node));
}

#endif // !_DESERIALIZER_H_
