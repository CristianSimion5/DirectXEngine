#ifndef _SHADER_H_
#define _SHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class Shader {
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, MatrixBufferType);

private:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3DBlob*, HWND, LPCWSTR);

	bool SetShaderParameters(ID3D11DeviceContext*, MatrixBufferType);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_Layout;
	ID3D11Buffer* m_MatrixBuffer;
};

#endif // !_SHADER_H_