#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d11.h>

#include <string>

#include <Shader.h>

class Texture {
public:
    Texture(ID3D11Device*, ID3D11DeviceContext*, std::wstring, HWND);
    ~Texture();
    
    void SetTexture(ID3D11DeviceContext*) const;

private:
    ID3D11ShaderResourceView* m_TextureView;
    ID3D11SamplerState* m_SamplerState;
};

#endif // !_TEXTURE_H_