#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <vector>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

// Data meant to correspond to constant buffers

struct MatrixBufferType {
	Matrix world;
	Matrix view;
	Matrix projection;
};

struct LightingMatrices {
	Matrix worldMatrix;
	Matrix inverseTransposeWorldMatrix;
	Matrix worldViewProjectionMatrix;
};

struct PhongMaterialProperties {
	// Emissive not usually used, make emissive component last
	PhongMaterialProperties(
		Vector4 a = Vector4(0.1f, 0.1f, 0.1f, 1.0f),
		Vector4 d = Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4 s = Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		float ss = 128.0f,
		Vector4 e = Vector4(0.0f, 0.0f, 0.0f, 1.0f))
		: emissive(e), ambient(a), diffuse(d), specular(s), specularStrength(ss) {}

	//PhongMaterialProperties(Vector4 a, Vector4 d, Vector4 s, float ss)
	//    : ambient(a), diffuse(d), specular(s), specularStrength(ss) {}

	Vector4 emissive;
	Vector4 ambient;
	Vector4 diffuse;
	Vector4 specular;
	float specularStrength;
	Vector3 PADDING;
};

struct LightStruct {
	Vector4 position;
	Color color;
	Vector3 attenuation;
	bool enabled;
};

struct LightProperties {
	Vector4 eyePosition;
	Vector4 globalAmbient;
	LightStruct lights[8];
	bool specularType;
	Vector3 PADDING;
};

// struct containing all types of data that can be sent to shaders
struct ShaderPayload {
	MatrixBufferType matrices;
	LightingMatrices lightMatrices;
	PhongMaterialProperties phongMaterial;
	LightProperties lightProperties;
	Matrix viewProjectionMatrix;
};


class Shader {
public:
	virtual ~Shader() = default;

	bool Initialize(ID3D11Device*, HWND, const std::wstring&, const std::wstring&);
	void Shutdown();
	void SetShader(ID3D11DeviceContext*);

	virtual bool SetShaderParameters(ID3D11DeviceContext*, ShaderPayload*) = 0;

protected:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3DBlob*, HWND, LPCWSTR);

	virtual std::vector<D3D11_INPUT_ELEMENT_DESC> GenerateInputLayout() = 0;
	virtual bool CreateConstantBuffers(ID3D11Device*) = 0;
	virtual void ShutdownConstantBuffers() = 0;

protected:
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_Layout;
};

class SimpleShader: public Shader {
public:
	virtual bool SetShaderParameters(ID3D11DeviceContext*, ShaderPayload*) override;

private:
	virtual std::vector<D3D11_INPUT_ELEMENT_DESC> GenerateInputLayout() override;
	virtual bool CreateConstantBuffers(ID3D11Device*) override;
	virtual void ShutdownConstantBuffers() override;

private:
	ID3D11Buffer* m_MatrixBuffer;
};

class PhongShader : public Shader {
public:
	virtual bool SetShaderParameters(ID3D11DeviceContext*, ShaderPayload*) override;

private:
	virtual std::vector<D3D11_INPUT_ELEMENT_DESC> GenerateInputLayout() override;
	virtual bool CreateConstantBuffers(ID3D11Device*) override;
	virtual void ShutdownConstantBuffers() override;

private:
	ID3D11Buffer* m_PerObjectConstantBuffer;
	ID3D11Buffer* m_LightPropertiesConstantBuffer;
};

#endif // !_SHADER_H_