#ifndef _SCENENODE_H_
#define _SCENENODE_H_

#include <vector>
#include <memory>

#include <SimpleMath.h>

#include "Model.hpp"

using namespace DirectX::SimpleMath;

class SceneNode {
public:
    SceneNode(const SceneNode* parent = nullptr, const Model* model = nullptr);
    //virtual ~SceneNode() = default;
    bool Render(ID3D11DeviceContext*, Shader*, const Matrix&, const Matrix&);
    void UpdateTransform();
    void AddChild(std::unique_ptr<SceneNode>&& child);

    void SetModel(const Model*);

public:
    Transform transform;
    std::vector<std::unique_ptr<SceneNode>> children;

private:
    // Observing pointers
    const SceneNode* m_Parent;
    const Model* m_Model;
};

#endif // !_SCENENODE_H_