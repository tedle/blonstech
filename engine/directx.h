#ifndef __DIRECTX_H__
#define __DIRECTX_H__

// Linking
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

// Includes
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class D3D
{
    
public:
    D3D();
    ~D3D();

    bool Init(int, int, bool, HWND, bool, float, float);
    void Finish();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    XMFLOAT4X4 GetProjectionMatrix();
    XMFLOAT4X4 GetWorldMatrix();
    XMFLOAT4X4 GetOrthoMatrix();

    void GetVideoCardInfo(char*, int&);

private:
    bool vsync_;
    int video_card_memory_;
    char video_card_desc_[128];
    IDXGISwapChain* swapchain_;
    ID3D11Device* device_;
    ID3D11DeviceContext* device_context_;
    ID3D11RenderTargetView* render_target_view_;
    ID3D11Texture2D* depth_stencil_buffer_;
    ID3D11DepthStencilState* depth_stencil_state_;
    ID3D11DepthStencilView* depth_stencil_view_;
    ID3D11RasterizerState* raster_state_;
    XMFLOAT4X4 projection_matrix_;
    XMFLOAT4X4 world_matrix_;
    XMFLOAT4X4 ortho_matrix_;
};
    
#endif