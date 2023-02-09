#include "SceneNode.h"

#include "FrustumCulling.h"
#include <ScriptingManager.h>

SceneNode::SceneNode(std::string _name, const SceneNode* parent, const Model* model)
    : name(_name), m_Parent(parent), m_Model(model) {}

bool SceneNode::Render(ID3D11DeviceContext* deviceContext, ShaderPayload* shaderPayload, Frustum* camFrustum) {
    bool renderSuccess;
    if (m_Model) {
        if (camFrustum) {
            if (m_Model->boundingSphere.IsOnFrustum(*camFrustum, transform.globalMatrix)) {
                culled = false;
                renderSuccess = m_Model->Render(deviceContext, shaderPayload, transform.globalMatrix);
                if (!renderSuccess) {
                    return false;
                }
            } else {
                culled = true;
            }
        }
        else {
            renderSuccess = m_Model->Render(deviceContext, shaderPayload, transform.globalMatrix);
            if (!renderSuccess) {
                return false;
            }
        }
    }

    for (auto& child : children) {
        child->Render(deviceContext, shaderPayload, camFrustum);
    }

    return true;
}

void SceneNode::UpdateTransform() {
    if (m_Parent) {
        transform.UpdateGlobalMatrix(m_Parent->transform.globalMatrix);
    } else {
        transform.UpdateGlobalMatrix(Matrix::Identity);
    }
    for (auto& child : children) {
        child->UpdateTransform();
    }
}

void SceneNode::Update(float deltaTime, ScriptingManager* scripting) {
    if (moving)
        dir = scripting->luaUpdate(transform.position, dir, deltaTime);

    for (auto& child : children) {
        child->Update(deltaTime, scripting);
    }
}

void SceneNode::AddChild(std::unique_ptr<SceneNode>&& child) {
    child->transform.UpdateGlobalMatrix(transform.globalMatrix);
    children.push_back(std::move(child));
}

void SceneNode::SetModel(const Model* model) {
    m_Model = model;
}

const Model* SceneNode::GetModel() const {
    return m_Model;
}

std::string SceneNode::GetType() {
    return "node";
}
