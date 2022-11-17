#ifndef _GRAPHICS_MANAGER_H_
#define _GRAPHICS_MANAGER_H_

#include <memory>

#include "Helpers.h"

// DirectXTK input helper classes
#include "Keyboard.h"
#include "Mouse.h"

#include "D3D11Manager.h"
#include "Shader.h"
#include "Model.hpp"
#include "Camera.h"

// Globals
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsManager {
public:
    GraphicsManager() = default;
    GraphicsManager(const GraphicsManager&) = default;
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
    std::unique_ptr<Camera> m_Camera;
    std::vector<std::unique_ptr<Model>> m_Models;
    std::unique_ptr<Shader> m_Shader;

    std::unique_ptr<DirectX::Keyboard> m_Keyboard;
    std::unique_ptr<DirectX::Mouse> m_Mouse;
};

#endif // !_GRAPHICS_MANAGER_H_