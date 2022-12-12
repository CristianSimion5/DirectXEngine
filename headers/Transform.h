#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

class Transform {
public:
    Transform(
        Vector3 position = Vector3::Zero,
        Vector3 rotation = Vector3::Zero,
        Vector3 scale = Vector3::One
    );

    Transform(
        const Matrix& parentGlobalMatrix,
        Vector3 position = Vector3::Zero,
        Vector3 rotation = Vector3::Zero,
        Vector3 scale = Vector3::One
    );

    Transform(
        const Matrix& parentGlobalMatrix,
        Matrix localMatrix
    );

    Matrix GetLocalMatrix();
    void UpdateGlobalMatrix(const Matrix&);

public:
    // Position, Rotation and Scale refer to local coordinates
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;

    Matrix globalMatrix;
};

#endif // !_TRANSFORM_H_