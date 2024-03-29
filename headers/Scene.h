#ifndef _SCENE_H_
#define _SCENE_H_

#include <map>
#include <string>
#include <memory>

#include "Helpers.h"

#include "Camera.h"
#include "Light.h"

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class ScriptingManager;

class Scene {
public:
    Scene(std::string, ID3D11Device*, ID3D11DeviceContext*);
    bool Initialize(int, int, HWND);
    void Shutdown();

    void Update(float, ScriptingManager*);
    bool Render();
    
    void HandleResize(int, int);
    
    Camera* GetMainCamera();
    const SceneNode* GetSceneRoot();

private:
    bool InitializeShaders();
    void InitializeTextures();
    void InitializeMaterials();
    bool InitializeModels();
    void InitializeScene(int, int);

public:
    std::string name;

private:
    Camera* m_MainCamera;
    std::map<std::string, Light*> m_Lights;
    std::map<std::string, std::unique_ptr<Model>> m_Models;
    std::map<std::string, std::unique_ptr<Texture>> m_Textures;
    std::map<std::string, std::unique_ptr<Material>> m_Materials;
    std::unique_ptr<SceneNode> m_SceneRoot;

    std::map<std::string, std::unique_ptr<Shader>> m_Shaders;
    ShaderPayload m_ShaderPayload;

    HWND m_hWnd;
    ID3D11Device* m_Device;
    ID3D11DeviceContext* m_DeviceContext;

    int m_ScreenWidth, m_ScreenHeight;

    friend class Serializer;
    friend class Deserializer;
};

#endif // !_SCENE_H_
