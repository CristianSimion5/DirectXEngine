#ifndef _D3D11_MANAGER_H_
#define _D3D11_MANAGER_H_

// Linking DirectX DLLs
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <DirectXMath.h>

#include <memory>

class D3D11Manager {
public:
    bool Initialize(int, int, bool, HWND, bool, float, float);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    DirectX::XMMATRIX& GetProjectionMatrix();
    DirectX::XMMATRIX& GetOrthoMatrix();

    void GetVideoCardInfo(char*, int&);

    bool ResizeSwapChain(int, int);
    void SetViewportAndProjections(int, int, float, float);

private:
    bool InitializeDXGI(int, int, HWND, bool);
    DXGI_RATIONAL QueryAdapter(int, int);
    bool CreateSwapChainAndDevice(int, int, HWND, DXGI_RATIONAL, bool);
    bool CreateRenderTargets(int, int);
    bool CreateDepthStencilAndRasterizerStates();

private:
    bool m_VsyncEnabled;
    int m_VideoCardMemory;
    char m_VideoCardDescription[128];
    IDXGISwapChain* m_SwapChain;
    ID3D11Device* m_Device;
    ID3D11DeviceContext* m_DeviceContext;
    ID3D11RenderTargetView* m_RenderTargetView;
    ID3D11Texture2D* m_DepthStencilBuffer;
    ID3D11DepthStencilState* m_DepthStencilState;
    ID3D11DepthStencilView* m_DepthStencilView;
    ID3D11RasterizerState* m_RasterizerState;
    DirectX::XMMATRIX m_ProjectionMatrix;
    DirectX::XMMATRIX m_OrthoMatrix;
};

#endif // !_D3D11_MANAGER_H_