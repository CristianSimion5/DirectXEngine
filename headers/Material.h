#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <memory>

#include "Shader.h"
#include "Texture.h"
#include "SimpleMath.h"

class Material {
public:
    Material(Shader*);
    virtual ~Material() = default;

    void SetShader(ID3D11DeviceContext*);

    virtual bool SetShaderPerMeshData(ID3D11DeviceContext*, ShaderPayload*) = 0;
    virtual bool SetShaderMaterialParameters(ID3D11DeviceContext*, ShaderPayload*) = 0;

protected:
    Shader* m_Shader;
};

class NormalAsColorMaterial : public Material {
public:
    NormalAsColorMaterial(Shader*);

    virtual bool SetShaderPerMeshData(ID3D11DeviceContext*, ShaderPayload*) override;
    virtual bool SetShaderMaterialParameters(ID3D11DeviceContext*, ShaderPayload*) override;
};

namespace PhongProperties {
    const PhongMaterialProperties DefaultMaterial;
    const PhongMaterialProperties GreenPlastic(
        Vector4(0.0f, 0.0f, 0.0f, 1.0f),
        Vector4(0.1f, 0.35f, 0.1f, 1.0f),
        Vector4(0.45f, 0.55f, 0.45f, 1.0f),
        32
    );
    const PhongMaterialProperties RedRubber(
        Vector4(0.05f, 0.0f, 0.0f, 1.0f),
        Vector4(0.5f, 0.4f, 0.4f, 1.0f),
        Vector4(0.7f, 0.04f, 0.04f, 1.0f),
        10
    );
    const PhongMaterialProperties Emerald(
        Vector4(0.0215f, 0.1745f, 0.0215f, 1.0f),
        Vector4(0.07568f, 0.61424f, 0.07568f, 1.0f),
        Vector4(0.633f, 0.727811f, 0.633f, 1.0f),
        76.8f);
    
}

class PhongMaterial: public Material {
public:
    PhongMaterial(ID3D11Device*, Shader*, const Texture*, PhongMaterialProperties);
    virtual ~PhongMaterial();

    virtual bool SetShaderPerMeshData(ID3D11DeviceContext*, ShaderPayload*) override;
    virtual bool SetShaderMaterialParameters(ID3D11DeviceContext*, ShaderPayload*) override;

private:
    const Texture* m_Texture;
    PhongMaterialProperties m_MaterialProperties;
    ID3D11Buffer* m_MaterialConstantBuffer;
};

#endif // !_MATERIAL_H_
