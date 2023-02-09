#ifndef _SCENENODE_H_
#define _SCENENODE_H_

#include <vector>
#include <memory>

#include <SimpleMath.h>

#include "Model.hpp"

using namespace DirectX::SimpleMath;

struct Frustum;
class ScriptingManager;

class SceneNode {
public:
    SceneNode(std::string _name, const SceneNode* parent = nullptr, const Model* model = nullptr);
    //virtual ~SceneNode() = default;
    bool Render(ID3D11DeviceContext*, ShaderPayload*, Frustum* = nullptr);
    void UpdateTransform();
    void Update(float deltaTime, ScriptingManager* scripting);
    void AddChild(std::unique_ptr<SceneNode>&& child);

    void SetModel(const Model*);
    const Model* GetModel() const;

    virtual std::string GetType();

public:
    std::string name;
    Transform transform;
    std::vector<std::unique_ptr<SceneNode>> children;
    bool culled = false;
    bool moving = false;

private:
    // Observing pointers
    const SceneNode* m_Parent;
    const Model* m_Model;
    
    float dir = -1.0f;

    friend class Serializer;
    friend class Deserializer;
    friend class GuiManager;
};

#endif // !_SCENENODE_H_