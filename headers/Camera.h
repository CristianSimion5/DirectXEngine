#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <SimpleMath.h>

#include "SceneNode.h"

class Camera : public SceneNode {
public:
    Camera(std::string _name, const SceneNode* = nullptr, const Model* = nullptr);

    void GenerateViewMatrix();
    void GenerateProjectionMatrices(int, int, float, float);

    const Matrix& GetViewMatrix();
    const Matrix& GetProjectionMatrix();
    const Matrix& GetOrthoMatrix();

    virtual std::string GetType() override;

private:
    Matrix m_ViewMatrix;
    Matrix m_ProjectionMatrix;
    Matrix m_OrthoMatrix;

    float m_FieldOfView;

    friend class Serializer;
    friend class Deserializer;
};

#endif // !_CAMERA_H_