#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <DirectXMath.h>

class Camera {
public:
    Camera();

    void SetPosition(DirectX::XMFLOAT3);
    //void SetRotation(DirectX::XMFLOAT3);
    void SetPitchYaw(float, float);
    
    void Move(DirectX::XMFLOAT3);
    void UpdatePitch(float);
    void UpdateYaw(float);

    DirectX::XMFLOAT3 GetPosition();
    //DirectX::XMFLOAT3 GetRotation();
    float GetPitch();
    float GetYaw();

    DirectX::XMMATRIX& GetViewMatrix();
    void Render(float);

private:
    DirectX::XMFLOAT3 m_Position;
    //DirectX::XMFLOAT3 m_Rotation;
    DirectX::XMMATRIX m_ViewMatrix;

    float m_Pitch;
    float m_Yaw;
};

#endif // !_CAMERA_H_