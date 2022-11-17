#include "Camera.h"

Camera::Camera() {
    m_Position = { 0.0f, 0.0f, 0.0f };
    //m_Rotation = { 0.0f, 0.0f, 0.0f };
    m_Pitch = 0.0f;
    m_Yaw = 0.0f;
}

void Camera::SetPosition(DirectX::XMFLOAT3 position) {
    m_Position = position;
}

void Camera::SetPitchYaw(float pitch, float yaw) {
    m_Pitch = pitch;
    m_Yaw = yaw;
}

void Camera::Move(DirectX::XMFLOAT3 moveDir) {
    m_Position.x += moveDir.x;
    m_Position.y += moveDir.y;
    m_Position.z += moveDir.z;
}

void Camera::UpdatePitch(float update) {
    m_Pitch += update;
}

void Camera::UpdateYaw(float update) {
    m_Yaw += update;
}

//void Camera::SetRotation(DirectX::XMFLOAT3 rotation) {
//    m_Rotation = rotation;
//}

DirectX::XMFLOAT3 Camera::GetPosition() {
    return m_Position;
}

float Camera::GetPitch() {
    return m_Pitch;
}

float Camera::GetYaw() {
    return m_Yaw;
}

//DirectX::XMFLOAT3 Camera::GetRotation() {
//    return m_Rotation;
//}

void Camera::Render(float deltaTime) {
    DirectX::XMFLOAT3 up;
    DirectX::XMFLOAT3 lookAt;
    DirectX::XMVECTOR upVector;
    DirectX::XMVECTOR positionVector;
    DirectX::XMVECTOR lookAtVector;
    /*float roll;
    float pitch;
    float yaw;*/
    DirectX::XMMATRIX rotationMatrix;

    up = { 0.0f, 1.0f, 0.0f };
    lookAt = { 0.0f, 0.0f, 1.0f };

    upVector = DirectX::XMLoadFloat3(&up);
    positionVector = DirectX::XMLoadFloat3(&m_Position);
    lookAtVector = DirectX::XMLoadFloat3(&lookAt);

    /*pitch = DirectX::XMConvertToRadians(m_Rotation.x);
    yaw = DirectX::XMConvertToRadians(m_Rotation.y);
    roll = DirectX::XMConvertToRadians(m_Rotation.z);*/

    rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f);

    upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);
    lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);

    lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

    m_ViewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

DirectX::XMMATRIX& Camera::GetViewMatrix() {
    return m_ViewMatrix;
}
