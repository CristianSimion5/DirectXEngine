#include "Light.h"

Light::Light(Color color, Vector3 attenuationCoef, bool enabled, const SceneNode* parent, const Model* model)
    : SceneNode(parent, model), m_Color(color), m_AttenuationCoef(attenuationCoef), m_Enabled(enabled) { }

LightStruct Light::GetStruct() const {
    LightStruct lightStruct;
    lightStruct.position = Vector4(transform.globalMatrix.m[3]);
    lightStruct.color = m_Color;
    lightStruct.attenuation = m_AttenuationCoef;
    lightStruct.enabled = m_Enabled;

    return lightStruct;
}

void Light::ToggleLight() {
    m_Enabled = !m_Enabled;
}