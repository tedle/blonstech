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

#ifndef BLONSTECH_GRAPHICS_RENDER_RENDERGL40_H_
#define BLONSTECH_GRAPHICS_RENDER_RENDERGL40_H_

// Linking
#pragma comment(lib, "opengl32.lib")

// Includes
#include <Windows.h>
#include <gl/GL.h>
// Public Includes
#include <blons/graphics/render/render.h>

namespace blons
{
class RenderGL40 : public Render
{
public:
    RenderGL40(units::pixel screen_width, units::pixel screen_height, bool vsync,
               HWND hwnd, bool fullscreen);
    ~RenderGL40() override;

    void BeginScene() override;
    void EndScene() override;

    BufferResource* MakeBufferResource() override;
    TextureResource* MakeTextureResource() override;
    ShaderResource* MakeShaderResource() override;

    bool Register3DMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                        Vertex* vertices, unsigned int vert_count,
                        unsigned int* indices, unsigned int index_count) override;
    bool Register2DMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                        Vertex* vertices, unsigned int vert_count,
                        unsigned int* indices, unsigned int index_count) override;
    bool RegisterTexture(TextureResource* texture, PixelData* pixel_data) override;
    bool RegisterShader(ShaderResource* program,
                        std::string vertex_filename, std::string pixel_filename,
                        ShaderAttributeList inputs) override;

    void RenderShader(ShaderResource* program, unsigned int index_count) override;

    void BindMeshBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer) override;
    void SetMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                     const Vertex* vertices, unsigned int vert_count,
                     const unsigned int* indices, unsigned int index_count);
    void UpdateMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                        const Vertex* vertices, unsigned int vert_offset, unsigned int vert_count,
                        const unsigned int* indices, unsigned int index_offset, unsigned int index_count) override;
    void MapMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                     Vertex** vertex_data, unsigned int** index_data) override;
    bool SetShaderInput(ShaderResource* program, const char* name, int value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, Matrix value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, Vector3 value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, Vector4 value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const TextureResource& value) override;

    bool SetDepthTesting(bool enable) override;

    void GetVideoCardInfo(char* buffer, int& len_buffer) override;

    // TODO: merge this without RegisterTexture(which should accept a pixel buffer)
    bool LoadPixelData(std::string filename, PixelData* pixel_data) override;

    void BindShader(GLuint shader);
    void UnbindShader();
    void UnmapBuffers();

private:
    unsigned int id_;
    void LogCompileErrors(GLuint resource, bool is_shader);
    bool vsync_;
    int video_card_memory_;
    std::string video_card_desc_;

    // API specific
    HDC device_context_;
    HGLRC render_context_;

    // Caching to prevent unnecessary API calls
    GLuint active_shader_;
    struct MappedBuffers
    {
        GLuint vertex = 0;
        Vertex* vertex_data = nullptr;
        GLuint index = 0;
        unsigned int* index_data = nullptr;
    } mapped_buffers_;
};
} // namespace blons
    
#endif // BLONSTECH_GRAPHICS_RENDER_RENDERGL40_H_