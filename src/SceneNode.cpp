#include "SceneNode.h"

SceneNode::SceneNode(std::string _name, const SceneNode* parent, const Model* model)
    : name(_name), m_Parent(parent), m_Model(model) {}

bool SceneNode::Render(ID3D11DeviceContext* deviceContext, ShaderPayload* shaderPayload) {
    bool renderSuccess;
    if (m_Model) {
        renderSuccess = m_Model->Render(deviceContext, shaderPayload, transform.globalMatrix);
        if (!renderSuccess) {
            return false;
        }
    }

    for (auto& child : children) {
        child->Render(deviceContext, shaderPayload);
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

void SceneNode::AddChild(std::unique_ptr<SceneNode>&& child) {
    child->transform.UpdateGlobalMatrix(transform.globalMatrix);
    children.push_back(std::move(child));
}

void SceneNode::SetModel(const Model* model) {
    m_Model = model;
}

std::string SceneNode::GetType() {
    return "node";
}
