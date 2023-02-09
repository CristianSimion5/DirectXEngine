#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d11.h>

#include <string>

#include <Shader.h>

class Texture {
public:
    Texture(std::string, ID3D11Device*, ID3D11DeviceContext*, std::string, HWND);
    ~Texture();
    
    void SetTexture(ID3D11DeviceContext*, UINT slot = 0) const;

public:
    std::string name;

private:
    ID3D11ShaderResourceView* m_TextureView;
    ID3D11SamplerState* m_SamplerState;

    const std::string m_Path;

    friend class Serializer;
    friend class Deserializer;
};

#endif // !_TEXTURE_H_