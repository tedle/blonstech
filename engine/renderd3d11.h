#ifndef BLONSTECH_RENDERD3D11_H_
#define BLONSTECH_RENDERD3D11_H_

// NOTE: D3D11 PIPELINE HAS FALLEN DECENTLY FAR BEHIND OPENGL PIPELINE AND WILL NOT COMPILE ATM

// Linking
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
// TODO: get rid of this dependancy eventually
#pragma comment(lib, "d3dcompiler.lib")

// Includes
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
// TODO: get rid of this w/ the other one
#include <d3dcompiler.h>
#include <fstream>
// Local Includes
// #include <DDSTextureLoader\DDSTextureLoader.h>
#include "math.h"
#include "render.h"

class BufferResourceD3D11 : public BufferResource
{
public:
    ~BufferResourceD3D11();

    ID3D11Buffer* p;
};

class TextureResourceD3D11 : public TextureResource
{
public:
    ~TextureResourceD3D11();

    ID3D11ShaderResourceView* p;
};

class ShaderResourceD3D11 : public ShaderResource
{
public:
    ~ShaderResourceD3D11();

    ID3D11VertexShader* vertex_shader_;
    ID3D11PixelShader* pixel_shader_;
    ID3D11InputLayout* layout_;
    ID3D11Buffer* matrix_buffer_;
    ID3D11SamplerState* sampler_state_;
};


class RenderD3D11 : public RenderAPI
{
    
public:
    RenderD3D11(int screen_width, int screen_height, bool vsync,
                HWND hwnd, bool fullscreen, float depth, float near);
    ~RenderD3D11();

    void BeginScene();
    void EndScene();

    BufferResource* CreateBufferResource();
    TextureResource* CreateTextureResource();
    ShaderResource* CreateShaderResource();

    bool RegisterMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                      Vertex* vertices, unsigned int vert_count,
                      unsigned int* indices, unsigned int index_count);
    void RegisterTexture();
    bool RegisterShader(ShaderResource* program,
                        WCHAR* vertex_filename, WCHAR* pixel_filename);

    void RenderShader(ShaderResource* program, int index_count);

    void SetModelBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer);
    bool SetShaderInputs(ShaderResource* program, TextureResource* texture,
                         Matrix world_matrix, Matrix view_matrix, Matrix proj_matrix);

    Matrix projection_matrix();
    Matrix ortho_matrix();

    void GetVideoCardInfo(char* buffer, int& len_buffer);

    // TODO: merge this without RegisterTexture(which should accept a pixel buffer)
    TextureResource* LoadDDSFile(WCHAR* filename);

private:
    void OutputShaderErrorMessage(ID3D10Blob*);
    bool vsync_;
    int video_card_memory_;
    std::string video_card_desc_;
    Matrix proj_matrix_;
    Matrix ortho_matrix_;

    // API specific
    IDXGISwapChain* swapchain_;
    ID3D11Device* device_;
    ID3D11DeviceContext* device_context_;
    ID3D11RenderTargetView* render_target_view_;
    ID3D11Texture2D* depth_stencil_buffer_;
    ID3D11DepthStencilState* depth_stencil_state_;
    ID3D11DepthStencilView* depth_stencil_view_;
    ID3D11RasterizerState* raster_state_;
};
    
#endif