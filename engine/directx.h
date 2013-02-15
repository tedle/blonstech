#ifndef __DIRECTX_H__
#define __DIRECTX_H__

// Globals
//#define _XM_NO_INTRINSICS_ // Allows unaligned matrices (matrix doesnt freak out when class member)

// Linking
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "d3dx10.lib")

// Includes
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class CD3D
{
    
public:
    CD3D();
    ~CD3D();

    bool Init(int, int, bool, HWND, bool, float, float);
    void Finish();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    XMMATRIX GetProjectionMatrix();
    XMMATRIX GetWorldMatrix();
    XMMATRIX GetOrthoMatrix();

    void GetVideoCardInfo(char*, int&);

private:
    bool m_vsync;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11RasterizerState* m_rasterState;
    XMMATRIX m_projectionMatrix;
    XMMATRIX m_worldMatrix;
    XMMATRIX m_orthoMatrix;
};
    
#endif