////////////////////////////////////////////////////////////////////////////////
/// blonstech
/// Copyright(c) 2014 Dominic Bowden
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files(the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions :
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#ifndef BLONSTECH_GRAPHICS_RENDER_RENDERD3D11_H_
#define BLONSTECH_GRAPHICS_RENDER_RENDERD3D11_H_

// NOTE: D3D11 PIPELINE HAS FALLEN DECENTLY FAR BEHIND OPENGL PIPELINE AND WILL NOT COMPILE ATM

// Linking
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
// TODO: get rid of this dependancy eventually
#pragma comment(lib, "d3dcompiler.lib")

// Includes
#include <d3d11.h>
// Public Includes
#include <blons/math/math.h>
#include <blons/graphics/render/render.h>

namespace blons
{
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


class RenderD3D11 : public Render
{
    
public:
    RenderD3D11(int screen_width, int screen_height, bool vsync,
                HWND hwnd, bool fullscreen);
    ~RenderD3D11();

    void BeginScene();
    void EndScene();

    BufferResource* MakeBufferResource();
    TextureResource* MakeTextureResource();
    ShaderResource* MakeShaderResource();

    bool RegisterMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                      Vertex* vertices, unsigned int vert_count,
                      unsigned int* indices, unsigned int index_count);
    void RegisterTexture();
    bool RegisterShader(ShaderResource* program,
                        WCHAR* vertex_filename, WCHAR* pixel_filename);

    void RenderShader(ShaderResource* program, int index_count);

    void BindModelBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer);
    bool SetShaderInputs(ShaderResource* program, TextureResource* texture,
                         Matrix world_matrix, Matrix view_matrix, Matrix proj_matrix);

    void GetVideoCardInfo(char* buffer, int& len_buffer);

    // TODO: merge this without RegisterTexture(which should accept a pixel buffer)
    TextureResource* LoadPixelData(WCHAR* filename);

private:
    void OutputShaderErrorMessage(ID3D10Blob*);
    bool vsync_;
    int video_card_memory_;
    std::string video_card_desc_;

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
} // namespace blons
    
#endif // BLONSTECH_GRAPHICS_RENDER_RENDERD3D11_H_