#ifndef _SCENENODE_H_
#define _SCENENODE_H_

#include <vector>
#include <memory>

#include <SimpleMath.h>

#include "Model.hpp"

using namespace DirectX::SimpleMath;

class SceneNode {
public:
    SceneNode(std::string _name, const SceneNode* parent = nullptr, const Model* model = nullptr);
    //virtual ~SceneNode() = default;
    bool Render(ID3D11DeviceContext*, ShaderPayload*);
    void UpdateTransform();
    void AddChild(std::unique_ptr<SceneNode>&& child);

    void SetModel(const Model*);

    virtual std::string GetType();

public:
    std::string name;
    Transform transform;
    std::vector<std::unique_ptr<SceneNode>> children;

private:
    // Observing pointers
    const SceneNode* m_Parent;
    const Model* m_Model;

    friend class Serializer;
    friend class Deserializer;
};

#endif // !_SCENENODE_H_