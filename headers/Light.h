#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <SimpleMath.h>

#include "SceneNode.h"

class Light : public SceneNode {
public:
    Light(Color, Vector3 = { 1.0f, 0.1f, 0.0f }, bool = true, const SceneNode* = nullptr, const Model* = nullptr);
    
    LightStruct GetStruct() const;
    void ToggleLight();

private:
    Color m_Color;
    Vector3 m_AttenuationCoef;
    bool m_Enabled;
};

#endif // !_LIGHT_H_