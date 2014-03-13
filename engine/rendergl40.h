#ifndef BLONSTECH_RENDERGL40_H_
#define BLONSTECH_RENDERGL40_H_

// Linking
#pragma comment(lib, "opengl32.lib")

// Includes
#include <Windows.h>
#include <gl/GL.h>
#include <fstream>
// Local Includes
#include "glfuncloader.h"
#include "math.h"
#include "render.h"

class BufferResourceGL40 : public BufferResource
{
public:
    //ID3D11Buffer* p;
};

class TextureResourceGL40 : public TextureResource
{
public:
    //ID3D11ShaderResourceView* p;
};

class ShaderResourceGL40 : public ShaderResource
{
public:
    /*
    ID3D11VertexShader* vertex_shader_;
    ID3D11PixelShader* pixel_shader_;
    ID3D11InputLayout* layout_;
    ID3D11Buffer* matrix_buffer_;
    ID3D11SamplerState* sampler_state_;*/
};


class RenderGL40 : public RenderAPI
{
    
public:
    RenderGL40();
    ~RenderGL40();

    bool Init(int screen_width, int screen_height, bool vsync,
              HWND hwnd, bool fullscreen, float depth, float near);
    void Finish();

    void BeginScene();
    void EndScene();

    void* CreateBufferResource();
    void* CreateTextureResource();
    void* CreateShaderResource();
    void DestroyBufferResource(BufferResource* buffer);
    void DestroyTextureResource(TextureResource* texture);
    void DestroyShaderResource(ShaderResource* shader);

    bool RegisterMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                      Vertex* vertices, unsigned int vert_count,
                      unsigned long* indices, unsigned int index_count);
    void RegisterTexture();
    bool RegisterShader(ShaderResource* program,
                        WCHAR* vertex_filename, WCHAR* pixel_filename);

    void RenderShader(ShaderResource* program, int index_count);

    void SetModelBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer);
    bool SetShaderInputs(ShaderResource* program, TextureResource* texture,
                         Matrix world_matrix, Matrix view_matrix, Matrix proj_matrix);

    Matrix GetProjectionMatrix();
    Matrix GetOrthoMatrix();

    void GetVideoCardInfo(char* buffer, int& len_buffer);

    // TODO: merge this without RegisterTexture(which should accept a pixel buffer)
    TextureResource* LoadDDSFile(WCHAR* filename);

private:
    bool vsync_;
    int video_card_memory_;
    char video_card_desc_[128];
    Matrix proj_matrix_;
    Matrix ortho_matrix_;

    // API specific
    HDC device_context_;
    HGLRC render_context_;
};
    
#endif