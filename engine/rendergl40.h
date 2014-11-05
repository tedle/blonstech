#ifndef BLONSTECH_RENDERGL40_H_
#define BLONSTECH_RENDERGL40_H_

// Linking
#pragma comment(lib, "opengl32.lib")

// Includes
#include <Windows.h>
#include <gl/GL.h>
// Local Includes
#include "render.h"

namespace blons
{
class BufferResourceGL40 : public BufferResource
{
public:
    ~BufferResourceGL40();

    GLuint buffer_, vertex_array_id_;
    enum BufferType { VERTEX_BUFFER, INDEX_BUFFER } type_;
};

class TextureResourceGL40 : public TextureResource
{
public:
    ~TextureResourceGL40();

    GLuint texture_, texture_unit_;
    //ID3D11ShaderResourceView* p;
};

class ShaderResourceGL40 : public ShaderResource
{
public:
    ~ShaderResourceGL40();

    GLuint program_;
    GLuint vertex_shader_;
    GLuint frag_shader_;
};


class RenderGL40 : public RenderAPI
{
    
public:
    RenderGL40(int screen_width, int screen_height, bool vsync,
               HWND hwnd, bool fullscreen, float depth, float near);
    ~RenderGL40();

    void BeginScene();
    void EndScene();

    BufferResource* CreateBufferResource();
    TextureResource* CreateTextureResource();
    ShaderResource* CreateShaderResource();

    bool RegisterMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                      Vertex* vertices, unsigned int vert_count,
                      unsigned int* indices, unsigned int index_count);
    bool RegisterTexture(TextureResource* texture, PixelData* pixel_data);
    bool RegisterShader(ShaderResource* program,
                        WCHAR* vertex_filename, WCHAR* pixel_filename,
                        ShaderAttributeList inputs);

    void RenderShader(ShaderResource* program, int index_count);

    void SetModelBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer);
    bool SetShaderInput(ShaderResource* program, const char* name, Matrix value);
    bool SetShaderInput(ShaderResource* program, const char* name, TextureResource* value);

    Matrix projection_matrix();
    Matrix ortho_matrix();

    void GetVideoCardInfo(char* buffer, int& len_buffer);

    // TODO: merge this without RegisterTexture(which should accept a pixel buffer)
    bool LoadPixelData(const char* filename, PixelData* pixel_data);

private:
    void LogCompileErrors(GLuint resource, bool is_shader);
    bool vsync_;
    int video_card_memory_;
    std::string video_card_desc_;
    Matrix proj_matrix_;
    Matrix ortho_matrix_;

    // API specific
    HDC device_context_;
    HGLRC render_context_;
};
} // namespace blons
    
#endif