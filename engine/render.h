#ifndef BLONSTECH_RENDER_H_
#define BLONSTECH_RENDER_H_

// Includes
#include <memory>
#include <string>
#include <Windows.h>
// Local Includes
#include "math.h"

struct Vertex
{
    Vector3 pos;
    Vector2 tex;
};

struct MatrixBuffer
{
    Matrix world;
    Matrix view;
    Matrix projection;
};

class BufferResource;
class ShaderResource;
class TextureResource;

class RenderAPI {

public:
    virtual ~RenderAPI() {};

    virtual bool Init(int screen_width, int screen_height, bool vsync,
                      HWND hwnd, bool fullscreen, float depth, float near)=0;
    virtual void Finish()=0;

    virtual void BeginScene()=0;
    virtual void EndScene()=0;

    virtual void* CreateBufferResource()=0;
    virtual void* CreateTextureResource()=0;
    virtual void* CreateShaderResource()=0;
    virtual void DestroyBufferResource(BufferResource* buffer)=0;
    virtual void DestroyTextureResource(TextureResource* texture)=0;
    virtual void DestroyShaderResource(ShaderResource* shader)=0;

    virtual bool RegisterMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                              Vertex* vertices, unsigned int vert_count,
                              unsigned int* indices, unsigned int index_count)=0;
    virtual void RegisterTexture()=0;
    virtual bool RegisterShader(ShaderResource* program,
                                WCHAR* vertex_filename, WCHAR* pixel_filename)=0;

    virtual void RenderShader(ShaderResource* program, int index_count)=0;

    virtual void SetModelBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)=0;
    virtual bool SetShaderInputs(ShaderResource* program, TextureResource* texture,
                                 Matrix world_matrix, Matrix view_matrix, Matrix proj_matrix)=0;

    virtual Matrix GetProjectionMatrix()=0;
    virtual Matrix GetOrthoMatrix()=0;

    virtual void GetVideoCardInfo(char* buffer, int& len_buffer)=0;

    // TODO: do this MANULLY in texture class later
    virtual TextureResource* LoadDDSFile(const char* filename)=0;

private:
    bool vsync_;
    int video_card_memory_;
    std::string video_card_desc_;
    Matrix proj_matrix_;
    Matrix ortho_matrix_;
};

extern std::unique_ptr<RenderAPI> g_render;

// TODO: make sure these are destroyed properly
class BufferResource {public: void* operator new(size_t s){return g_render->CreateBufferResource();}};
class TextureResource{public: void* operator new(size_t s){return g_render->CreateTextureResource();}};
class ShaderResource {public: void* operator new(size_t s){return g_render->CreateShaderResource();}};

#endif