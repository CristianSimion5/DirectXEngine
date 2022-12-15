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
	Shader(const std::string&, const std::string&, const std::string&);
	virtual ~Shader() = default;

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void SetShader(ID3D11DeviceContext*);

	virtual std::string GetType() = 0;
	virtual bool SetShaderParameters(ID3D11DeviceContext*, ShaderPayload*) = 0;

protected:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3DBlob*, HWND, LPCWSTR);

	virtual std::vector<D3D11_INPUT_ELEMENT_DESC> GenerateInputLayout() = 0;
	virtual bool CreateConstantBuffers(ID3D11Device*) = 0;
	virtual void ShutdownConstantBuffers() = 0;

public:
	std::string name;

protected:
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_Layout;

	const std::string m_VsPath;
	const std::string m_PsPath;

	friend class Serializer;
	friend class Deserializer;
};

class SimpleShader: public Shader {
public:
	using Shader::Shader;

	virtual std::string GetType() override;
	virtual bool SetShaderParameters(ID3D11DeviceContext*, ShaderPayload*) override;

private:
	virtual std::vector<D3D11_INPUT_ELEMENT_DESC> GenerateInputLayout() override;
	virtual bool CreateConstantBuffers(ID3D11Device*) override;
	virtual void ShutdownConstantBuffers() override;

private:
	ID3D11Buffer* m_MatrixBuffer;

	friend class Serializer;
	friend class Deserializer;
};

class PhongShader : public Shader {
public:
	using Shader::Shader;

	virtual std::string GetType() override;
	virtual bool SetShaderParameters(ID3D11DeviceContext*, ShaderPayload*) override;

private:
	virtual std::vector<D3D11_INPUT_ELEMENT_DESC> GenerateInputLayout() override;
	virtual bool CreateConstantBuffers(ID3D11Device*) override;
	virtual void ShutdownConstantBuffers() override;

private:
	ID3D11Buffer* m_PerObjectConstantBuffer;
	ID3D11Buffer* m_LightPropertiesConstantBuffer;

	friend class Serializer;
	friend class Deserializer;
};

#endif // !_SHADER_H_