#ifndef _GRAPHICS_MANAGER_H_
#define _GRAPHICS_MANAGER_H_

#include <memory>
#include <map>

#include "Helpers.h"

// DirectXTK input helper classes
#include "Keyboard.h"
#include "Mouse.h"

#include "D3D11Manager.h"
#include "Shader.h"
#include "Model.hpp"
#include "Texture.h"
#include "Material.h"
#include "SceneNode.h"
#include "Camera.h"
#include "Light.h"
#include "Scene.h"

// Globals
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;

class GraphicsManager {
public:
    GraphicsManager() = default;
    GraphicsManager(const GraphicsManager&) = delete;
    ~GraphicsManager() = default;

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame(float);
    bool HandleResize(int, int);

private:

    bool Render(float);
    void ProcessInput(float);

private:
    const float CAMERA_SPEED = 5.0f;
    const float LOOK_SPEED = 1.5f;

    std::unique_ptr<D3D11Manager> m_d3d;
   
    HWND m_hWnd;
    std::unique_ptr<DirectX::Keyboard> m_Keyboard;
    std::unique_ptr<DirectX::Mouse> m_Mouse;
    
    std::unique_ptr<Scene> m_Scene;
};

#endif // !_GRAPHICS_MANAGER_H_