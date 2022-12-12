#include "Camera.h"

Camera::Camera(const SceneNode* parent, const Model* model)
    : SceneNode(parent, model) {
    transform.position = Vector3::Zero;
    transform.rotation = Vector3::Zero;
}

const Matrix& Camera::GetViewMatrix() {
    return m_ViewMatrix;
}

const Matrix& Camera::GetProjectionMatrix() {
    return m_ProjectionMatrix;
}

const Matrix& Camera::GetOrthoMatrix() {
    return m_OrthoMatrix;
}

void Camera::GenerateViewMatrix() {
    Vector3 up = Vector3::Up;
    Vector3 lookAt = Vector3::Backward;
    Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(transform.rotation);

    up = Vector3::Transform(up, rotationMatrix);
    lookAt = Vector3::Transform(lookAt, rotationMatrix);
    lookAt += transform.position;

    m_ViewMatrix = DirectX::XMMatrixLookAtLH(transform.position, lookAt, up);
}

void Camera::GenerateProjectionMatrices(int screenWidth, int screenHeight, float screenDepth, float screenNear) {
    float fieldOfView;
    float screenAspect;

    fieldOfView = DirectX::XM_PIDIV4;
    screenAspect = 1.0f * screenWidth / screenHeight;

    m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect,
        screenNear, screenDepth);

    m_OrthoMatrix = DirectX::XMMatrixOrthographicLH(1.0f * screenWidth, 1.0f * screenHeight,
        screenNear, screenDepth);
}
