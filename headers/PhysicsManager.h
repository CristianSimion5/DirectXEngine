#ifndef _PHYSICS_MANAGER_H_
#define _PHYSICS_MANAGER_H_

#include "Scene.h"

#include <set>

class PhysicsManager {
public:
    void Update(Scene* scene) {
        collisions.clear();
        std::vector<std::pair<const std::string, const BoundingSphere>> prevNamedSpheres;
        DetectCollision(scene->GetSceneRoot(), prevNamedSpheres);
    }

    void DetectCollision(const SceneNode* currentNode, std::vector<std::pair<const std::string, const BoundingSphere>>& prevNamedSpheres) {
        const Model* currentModel = currentNode->GetModel();
        if (currentModel != nullptr && currentNode->name != "ground" && currentNode->name != "wall") {
            BoundingSphere currentSphere(currentModel->boundingSphere);
            currentSphere.center = Vector3::Transform(currentSphere.center, currentNode->transform.globalMatrix);
            currentSphere.radius *= std::max(std::max(currentNode->transform.scale.x, currentNode->transform.scale.y), currentNode->transform.scale.z);
            for (const auto& prevNamedSphere : prevNamedSpheres) {
                if (CheckSphereSphereIntersection(currentSphere, prevNamedSphere.second)) {
                    // Collision resolution
                    collisions.insert({ currentNode->name, prevNamedSphere.first });
                }
            }

            prevNamedSpheres.push_back({currentNode->name, currentSphere});
        }

        for (const auto& childNode : currentNode->children) {
            DetectCollision(childNode.get(), prevNamedSpheres);
        }
    }

    bool CheckSphereSphereIntersection(const BoundingSphere& A, const BoundingSphere& B) {
        return (A.radius + B.radius) * (A.radius + B.radius) > Vector3::DistanceSquared(A.center, B.center);
    }

public:
    std::set<std::pair<std::string, std::string>> collisions;
};

#endif // !_PHYSICS_MANAGER_H_