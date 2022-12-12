#include "Transform.h"

Transform::Transform(const Matrix& parentGlobalMatrix, Vector3 position, Vector3 rotation, Vector3 scale)
    : position(position), rotation(rotation), scale(scale) {
    UpdateGlobalMatrix(parentGlobalMatrix);
}

Transform::Transform(Vector3 position, Vector3 rotation, Vector3 scale)
    : Transform(Matrix::Identity, position, rotation, scale) {}

Transform::Transform(const Matrix& parentGlobalMatrix, Matrix localMatrix) {
    Quaternion quatRot;
    localMatrix.Decompose(scale, quatRot, position);
    rotation = quatRot.ToEuler();
    globalMatrix = localMatrix * parentGlobalMatrix;
}

Matrix Transform::GetLocalMatrix() {
    Matrix translate = Matrix::CreateTranslation(position);
    Matrix rotate = Matrix::CreateFromYawPitchRoll(rotation);
    Matrix scaling = Matrix::CreateScale(scale);
    return scaling * rotate * translate;
}

void Transform::UpdateGlobalMatrix(const Matrix& parentGlobalMatrix) {
    globalMatrix = GetLocalMatrix() * parentGlobalMatrix;
}
