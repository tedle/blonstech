#ifndef BLONSTECH_RENDER_H_
#define BLONSTECH_RENDER_H_

// Includes
#include <memory>
#include <string>
#include <vector>
#include <Windows.h>
// Local Includes
#include "math.h"
#include "loggeride.h"

struct Vertex
{
    Vector3 pos;
    Vector2 tex;
    Vector3 norm;
    // needed for efficient std::map lookups
    bool operator< (const Vertex vert) const {return memcmp(this, &vert, sizeof(Vertex))>0;}
};

struct MatrixBuffer
{
    Matrix world;
    Matrix view;
    Matrix projection;
};

class BufferResource {};
class ShaderResource {};
class TextureResource {};

typedef std::pair<unsigned int, std::string> ShaderAttribute;
typedef std::vector<ShaderAttribute> ShaderAttributeList;

class RenderAPI {

public:
    virtual ~RenderAPI() {};

    virtual bool Init(int screen_width, int screen_height, bool vsync,
                      HWND hwnd, bool fullscreen, float depth, float near)=0;
    virtual void Finish()=0;

    virtual void BeginScene()=0;
    virtual void EndScene()=0;

    virtual BufferResource* CreateBufferResource()=0;
    virtual TextureResource* CreateTextureResource()=0;
    virtual ShaderResource* CreateShaderResource()=0;
    virtual void DestroyBufferResource(BufferResource* buffer)=0;
    virtual void DestroyTextureResource(TextureResource* texture)=0;
    virtual void DestroyShaderResource(ShaderResource* shader)=0;

    virtual bool RegisterMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                              Vertex* vertices, unsigned int vert_count,
                              unsigned int* indices, unsigned int index_count)=0;
    virtual void RegisterTexture()=0;
    virtual bool RegisterShader(ShaderResource* program,
                                WCHAR* vertex_filename, WCHAR* pixel_filename,
                                ShaderAttributeList inputs)=0;

    virtual void RenderShader(ShaderResource* program, int index_count)=0;

    virtual void SetModelBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)=0;
    virtual bool SetShaderInput(ShaderResource* program, const char* name, Matrix value)=0;
    virtual bool SetShaderInput(ShaderResource* program, const char* name, TextureResource* value)=0;

    virtual Matrix projection_matrix()=0;
    virtual Matrix ortho_matrix()=0;

    virtual void GetVideoCardInfo(char* buffer, int& len_buffer)=0;

    // TODO: do this MANULLY in texture class later
    virtual TextureResource* LoadDDSFile(const char* filename)=0;

protected:
    bool vsync_;
    int video_card_memory_;
    std::string video_card_desc_;
    Matrix proj_matrix_;
    Matrix ortho_matrix_;
};

// To help readbility
typedef std::unique_ptr<RenderAPI> RenderContext;

#endif