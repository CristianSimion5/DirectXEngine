#ifndef _FRUSTUM_CULLING_H_
#define _FRUSTUM_CULLING_H_

#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class Camera;

struct Frustum {
    Frustum(Camera& camera, float aspectRatio, float zNear, float zFar);

    Plane top;
    Plane bottom;
    Plane right;
    Plane left;
    Plane farP;
    Plane nearP;
};

struct BoundingSphere {
    BoundingSphere();
    BoundingSphere(Vector3 _center, float _radius);
    bool IsOnFrustum(const Frustum& camFrustum, Matrix& globalMatrix) const;
    bool IsOnForwardPlane(const Plane& camPlane);

    Vector3 center;
    float radius;
};

#endif // !_FRUSTUM_CULLING_H_
