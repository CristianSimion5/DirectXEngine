#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <SimpleMath.h>

#include "SceneNode.h"

class Light : public SceneNode {
public:
    Light(std::string _name, Color, Vector3 = { 1.0f, 0.1f, 0.0f }, bool = true, 
        const SceneNode* = nullptr, const Model* = nullptr);
    
    LightStruct GetStruct() const;
    void ToggleLight();

    virtual std::string GetType() override;

private:
    Color m_Color;
    Vector3 m_AttenuationCoef;
    bool m_Enabled;

    friend class Serializer;
    friend class Deserializer;
    friend class GuiManager;
};

#endif // !_LIGHT_H_