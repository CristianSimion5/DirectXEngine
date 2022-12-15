#include "FrustumCulling.h"

Frustum::Frustum(Camera& camera, float aspectRatio, float zNear, float zFar) {
    const float halfVertical = zFar * tanf(camera.GetFov() * 0.5f);
    const float halfHorizontal = halfVertical * aspectRatio;

    Vector3 camScale, camPosition;
    Quaternion camRot;
    camera.transform.globalMatrix.Decompose(camScale, camRot, camPosition);
    // Get camera forward direction
    Vector3 forwardCam = Vector3::Backward;
    forwardCam = Vector3::Transform(forwardCam, camRot);
    Vector3 upCam = Vector3::Up;
    upCam = Vector3::Transform(upCam, camRot);
    Vector3 rightCam = Vector3::Right;
    rightCam = Vector3::Transform(rightCam, camRot);

    const Vector3 forwardMultFar = forwardCam * zFar;

    nearP = Plane(camPosition + zNear * forwardCam, forwardCam);
    farP = Plane(camPosition + forwardMultFar, -forwardCam);
    Vector3 cross = upCam.Cross(forwardMultFar + rightCam * halfHorizontal);
    cross.Normalize();
    right = Plane(camPosition, -cross);

    cross = (forwardMultFar - rightCam * halfHorizontal).Cross(upCam);
    cross.Normalize();
    left = Plane(camPosition, -cross);

    cross = rightCam.Cross(forwardMultFar - upCam * halfVertical);
    cross.Normalize();
    top = Plane(camPosition, -cross);

    cross = (forwardMultFar + upCam * halfVertical).Cross(rightCam);
    cross.Normalize();
    bottom = Plane(camPosition, -cross);
}

BoundingSphere::BoundingSphere(Vector3 _center, float _radius) : center(_center), radius(_radius) {}

bool BoundingSphere::IsOnFrustum(const Frustum& camFrustum, Matrix& globalMatrix) {
    Vector3 scale, _pos;
    Quaternion _quat;
    globalMatrix.Decompose(scale, _quat, _pos);
    Vector3 worldCenter = Vector3::Transform(center, globalMatrix);
    const float maxScale = std::max(std::max(scale.x, scale.y), scale.z);

    BoundingSphere worldSphere(worldCenter, radius * (maxScale * 1.0f));

   /* bool a1 = worldSphere.IsOnForwardPlane(camFrustum.left);
    bool a2 = worldSphere.IsOnForwardPlane(camFrustum.right);
    bool a3 = worldSphere.IsOnForwardPlane(camFrustum.farP);
    bool a4 = worldSphere.IsOnForwardPlane(camFrustum.nearP);
    bool a5 = worldSphere.IsOnForwardPlane(camFrustum.top);
    bool a6 = worldSphere.IsOnForwardPlane(camFrustum.bottom);

    return a1 && a2 && a3 && a4 && a5 && a6;*/
      
    return
        worldSphere.IsOnForwardPlane(camFrustum.left) &&
        worldSphere.IsOnForwardPlane(camFrustum.right) &&
        worldSphere.IsOnForwardPlane(camFrustum.farP) &&
        worldSphere.IsOnForwardPlane(camFrustum.nearP) &&
        worldSphere.IsOnForwardPlane(camFrustum.top) &&
        worldSphere.IsOnForwardPlane(camFrustum.bottom);
}

bool BoundingSphere::IsOnForwardPlane(const Plane& camPlane) {
    return (camPlane.DotNormal(center) + camPlane.D()) > -radius;
}