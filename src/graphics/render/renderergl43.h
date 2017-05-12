////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#ifndef BLONSTECH_GRAPHICS_RENDER_RENDERERGL43_H_
#define BLONSTECH_GRAPHICS_RENDER_RENDERERGL43_H_

// Linking
#pragma comment(lib, "opengl32.lib")

// Includes
#include <gl/GL.h>
// Public Includes
#include <blons/graphics/render/renderer.h>
#include <blons/system/client.h>

namespace blons
{
class RendererGL43 : public Renderer
{
public:
    RendererGL43(Client::Info screen_info, bool vsync, bool fullscreen);
    ~RendererGL43() override;

    void BeginScene(Vector4 clear_colour) override;
    void EndScene() override;

    BufferResource* MakeBufferResource() override;
    FramebufferResource* MakeFramebufferResource() override;
    TextureResource* MakeTextureResource() override;
    ShaderResource* MakeShaderResource() override;
    ShaderDataResource* MakeShaderDataResource() override;

    BufferResource* RegisterMesh(Vertex* vertices, unsigned int vert_count,
                                 unsigned int* indices, unsigned int index_count,
                                 DrawMode draw_mode) override;
    FramebufferResource* RegisterFramebuffer(units::pixel width, units::pixel height,
                                             std::vector<TextureType> formats, bool store_depth) override;
    TextureResource* RegisterTexture(PixelData* pixel_data) override;
    TextureResource* RegisterTexture(PixelData3D* pixel_data) override;
    TextureResource* RegisterTexture(PixelDataCubemap* pixel_data) override;
    ShaderResource* RegisterShader(ShaderSourceList source, ShaderAttributeList inputs) override;
    ShaderResource* RegisterComputeShader(ShaderSourceList source) override;
    ShaderDataResource* RegisterShaderData(const void* data, std::size_t size) override;

    void RenderShader(ShaderResource* program, unsigned int index_count) override;
    void RenderShaderInstanced(ShaderResource* program, unsigned int index_count, unsigned int instance_count) override;
    void RunComputeShader(ShaderResource* program, unsigned int groups_x,
                          unsigned int groups_y, unsigned int groups_z) override;

    void BindFramebuffer(FramebufferResource* frame_buffer) override;
    void SetFramebufferColourTextures(FramebufferResource* frame_buffer, const std::vector<const TextureResource*>& colour_textures, unsigned int mip_level) override;
    void SetFramebufferDepthTexture(FramebufferResource* frame_buffer, const TextureResource* depth_texture, unsigned int mip_level) override;
    std::vector<const TextureResource*> FramebufferTextures(FramebufferResource* frame_buffer) override;
    const TextureResource* FramebufferDepthTexture(FramebufferResource* frame_buffer) override;
    void BindMeshBuffer(BufferResource* buffer) override;
    void SetMeshData(BufferResource* buffer,
                     const Vertex* vertices, unsigned int vert_count,
                     const unsigned int* indices, unsigned int index_count);
    void UpdateMeshData(BufferResource* buffer,
                        const Vertex* vertices, unsigned int vert_offset, unsigned int vert_count,
                        const unsigned int* indices, unsigned int index_offset, unsigned int index_count) override;
    void MapMeshData(BufferResource* buffer,
                     Vertex** vertex_data, unsigned int** index_data) override;
    void SetTextureData(TextureResource* texture, PixelData* pixels, unsigned int mip_level) override;
    void SetTextureData(TextureResource* texture, PixelData3D* pixels, unsigned int mip_level) override;
    void SetTextureData(TextureResource* texture, PixelDataCubemap* pixels, unsigned int mip_level) override;
    PixelData GetTextureData(const TextureResource* texture, unsigned int mip_level) override;
    PixelData3D GetTextureData3D(const TextureResource* texture, unsigned int mip_level) override;
    PixelDataCubemap GetTextureDataCubemap(const TextureResource* texture, unsigned int mip_level) override;
    void MakeTextureMipmaps(TextureResource* texture) override;
    void SetTextureMipmapRange(TextureResource* texture, int min_level, int max_level) override;
    void SetShaderData(ShaderDataResource* data_handle, std::size_t offset, std::size_t length, const void* data) override;
    void GetShaderData(ShaderDataResource* data_handle, void* data) override;

    bool SetShaderInput(ShaderResource* program, const char* name, const float value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const int value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const Matrix value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const Vector2 value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const Vector3 value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const Vector4 value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const TextureResource* value, unsigned int texture_index) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const ShaderDataResource* value) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const float* value, std::size_t elements) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const int* value, std::size_t elements) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const Matrix* value, std::size_t elements) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const Vector2* value, std::size_t elements) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const Vector3* value, std::size_t elements) override;
    bool SetShaderInput(ShaderResource* program, const char* name, const Vector4* value, std::size_t elements) override;
    bool SetShaderOutput(ShaderResource* program, const char* name, TextureResource* value, unsigned int texture_index, unsigned int mip_level) override;

    bool SetBlendMode(BlendMode mode) override;
    bool SetCullMode(CullMode mode) override;
    bool SetDepthTesting(bool enable) override;
    bool SetViewport(units::pixel x, units::pixel y, units::pixel width, units::pixel height) override;

    void VideoCardInfo(char* buffer, int& len_buffer) override;

    bool IsDepthBufferRangeZeroToOne() const override;

    // TODO: merge this without RegisterTexture(which should accept a pixel buffer)
    bool LoadPixelData(std::string filename, PixelData* pixel_data) override;

    void BindShader(GLuint shader);
    void UnbindShader();
    void UnmapBuffers();

private:
    Client::Info screen_;
    void LogCompileErrors(GLuint resource, bool is_shader);
    void InitializeDebugOutput();
    bool vsync_;
    int video_card_memory_;
    std::string video_card_desc_;

    // API specific
    HDC device_context_;
    HGLRC render_context_;

    // Caching to prevent unnecessary API calls
    GLuint active_shader_;
    GLuint active_framebuffer_;
    GLenum draw_mode_;
    bool depth_testing_;
    struct MappedBuffers
    {
        GLuint vertex = 0;
        Vertex* vertex_data = nullptr;
        GLuint index = 0;
        unsigned int* index_data = nullptr;
    } mapped_buffers_;
};
} // namespace blons
    
#endif // BLONSTECH_GRAPHICS_RENDER_RENDERERGL43_H_