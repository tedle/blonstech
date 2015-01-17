////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
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

#ifndef BLONSTECH_GRAPHICS_RENDER_RENDER_H_
#define BLONSTECH_GRAPHICS_RENDER_RENDER_H_

// Includes
#include <memory>
#include <string>
#include <vector>
// Public Includes
#include <blons/math/math.h>
#include <blons/debug/console.h>
#include <blons/debug/log.h>

namespace blons
{
// TODO: Unify this struct and PixelData::Format
struct TextureHint
{
    enum Format
    {
        NONE,
        A8,
        R8G8B8,
        R32G32B32,
        R16G16,
        R8G8B8A8,
        R32G32B32A32
    } format;
    enum Filter
    {
        NEAREST,
        LINEAR
    } filter;
};

enum BlendMode
{
    ADDITIVE,
    ALPHA
};

enum CullMode
{
    ENABLE_CCW,
    ENABLE_CW,
    DISABLE
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Stores the vertices and indices of a mesh
////////////////////////////////////////////////////////////////////////////////
struct MeshData
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Vector of vertices
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<Vertex> vertices;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Vector of indices
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<unsigned int> indices;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Holds raw pixel data and format info of a texture
////////////////////////////////////////////////////////////////////////////////
struct PixelData
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Raw pixel data in bytes, one value per byte corresponding to
    /// PixelData::BitDepth
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<unsigned char> pixels;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Width of the texture in pixels
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel width;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Height of the texture in pixels
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel height;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines the pixel format and filtering of a texture
    ////////////////////////////////////////////////////////////////////////////////
    TextureHint hint;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines the format and rendering options of a texture
    ////////////////////////////////////////////////////////////////////////////////
    enum Compression
    {
        AUTO,      ///< Compresses to DXT5 & generates mipmaps
        DDS,       ///< Uses mipmaps & compression from image file
        RAW        ///< Will not generate mipmaps, will not compress on GPU
    } compression; ///< \copybrief Format

    PixelData() {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs a full copy of the given PixelData, including a byte for
    /// byte copy of the pixel buffer
    ////////////////////////////////////////////////////////////////////////////////
    PixelData(const PixelData& p)
    {
        this->width = p.width;
        this->height = p.height;
        this->hint = p.hint;
        this->compression = p.compression;
        int bits;
        switch (this->hint.format)
        {
        case TextureHint::A8:
            bits = 8;
            break;
        case TextureHint::R8G8B8:
            bits = 24;
            break;
        case TextureHint::R16G16:
        case TextureHint::R8G8B8A8:
            bits = 32;
            break;
        case TextureHint::NONE:
        default:
            assert(false);
            break;
        }
        std::size_t texture_size = p.width * p.height * (bits / 8);
        this->pixels.reset(new unsigned char[texture_size]);
        memcpy(this->pixels.get(), p.pixels.get(), texture_size);
    }
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Contains identifying data to render data to the screen
////////////////////////////////////////////////////////////////////////////////
class BufferResource
{
public:
    virtual ~BufferResource() {};
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains identifying data to render data to various textures
////////////////////////////////////////////////////////////////////////////////
class FramebufferResource
{
public:
    virtual ~FramebufferResource() {};
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains identifying data to activate and ineract with shaders
////////////////////////////////////////////////////////////////////////////////
class ShaderResource
{
public:
    virtual ~ShaderResource() {};
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains identifying data to bind textures
////////////////////////////////////////////////////////////////////////////////
class TextureResource
{
public:
    virtual ~TextureResource() {};
};

////////////////////////////////////////////////////////////////////////////////
/// \brief List of valid shader attributes indices
////////////////////////////////////////////////////////////////////////////////
enum ShaderAttributeIndex
{
    POS,       ///< Position input
    TEX,       ///< Diffuse texture UV input
    LIGHT_TEX, ///< Lightmap texture UV input
    NORMAL,    ///< Normal input
    TANGENT,   ///< Tangent input
    BITANGENT  ///< Bitangent input
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Holds a shader attribute's index and name
////////////////////////////////////////////////////////////////////////////////
typedef std::pair<ShaderAttributeIndex, std::string> ShaderAttribute;
////////////////////////////////////////////////////////////////////////////////
/// \brief Holds a list of shader attributes
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<ShaderAttribute> ShaderAttributeList;
////////////////////////////////////////////////////////////////////////////////
/// \brief Handle to a rendering context
////////////////////////////////////////////////////////////////////////////////
typedef std::unique_ptr<class Render> RenderContext;

////////////////////////////////////////////////////////////////////////////////
/// \brief Class for interfacing with a graphics API
////////////////////////////////////////////////////////////////////////////////
class Render
{
public:
    virtual ~Render() {};

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Called at the beginning of each frame, allows for any necessary setup
    ///
    /// \param clear_colour The colour to fill empty space with
    ////////////////////////////////////////////////////////////////////////////////
    virtual void BeginScene(Vector4 clear_colour)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Called at the end of each frame, allows for any necessary teardown
    /// and framebuffer swapping
    ////////////////////////////////////////////////////////////////////////////////
    virtual void EndScene()=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates an implementation specific BufferResource to be stored as its
    /// base class
    ///
    /// \return New BufferResource
    ////////////////////////////////////////////////////////////////////////////////
    virtual BufferResource* MakeBufferResource()=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates an implementation specific FramebufferResource to be stored
    /// as its base class
    ///
    /// \return New FramebufferResource
    ////////////////////////////////////////////////////////////////////////////////
    virtual FramebufferResource* MakeFramebufferResource()=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates an implementation specific TextureResource to be stored as its
    /// base class
    ///
    /// \return New TextureResource
    ////////////////////////////////////////////////////////////////////////////////
    virtual TextureResource* MakeTextureResource()=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates an implementation specific ShaderResource to be stored as its
    /// base class
    ///
    /// \return New ShaderResource
    ////////////////////////////////////////////////////////////////////////////////
    virtual ShaderResource* MakeShaderResource()=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a pair of BufferResource%s and binds them to the graphics API
    /// permitting their use for rendering calls. Sets up shader inputs using the
    /// entirety of supplied vertex data
    ///
    /// \param vertex_buffer Buffer for vertices to bind to
    /// \param index_buffer Buffer for indices to bind to
    /// \param vertices Vertices to be bound to buffer, may be nullptr if vert_count
    /// and index_count are 0
    /// \param vert_count Number of vertices to be bound to buffer, may be 0 for an
    /// empty mesh
    /// \param indices Indices to be bound to buffer, may be nullptr if vert_count
    /// and index_count are 0
    /// \param index_count Number of indices to be bound to buffer, may be 0 for an
    /// empty mesh
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool Register3DMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                Vertex* vertices, unsigned int vert_count,
                                unsigned int* indices, unsigned int index_count)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a pair of BufferResource%s and binds them to the graphics API
    /// permitting their use for rendering calls. Sets up shader inputs using a
    /// subset of the supplied vertex data useful for 2D rendering. This includes
    /// the X & Y coordinates of the vertex position as well as UV texture
    ///
    /// \param vertex_buffer Buffer for vertices to bind to
    /// \param index_buffer Buffer for indices to bind to
    /// \param vertices Vertices to be bound to buffer, may be nullptr if vert_count
    /// and index_count are 0
    /// \param vert_count Number of vertices to be bound to buffer, may be 0 for an
    /// empty mesh
    /// \param indices Indices to be bound to buffer, may be nullptr if vert_count
    /// and index_count are 0
    /// \param index_count Number of indices to be bound to buffer, may be 0 for an
    /// empty mesh
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool Register2DMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                Vertex* vertices, unsigned int vert_count,
                                unsigned int* indices, unsigned int index_count)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a FramebufferResource and binds it to the graphics API
    /// permitting its use for rendering calls. Expects an output to the supplied
    /// number of render targets.
    ///
    /// \param frame_buffer Framebuffer for to bind
    /// \param width Width of the textures to render to in pixels
    /// \param height Height of the textures to render to in pixels
    /// \param formats List of formatted textures to output to
    /// \param store_depth Store the depth buffer from each render to a texture
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool RegisterFramebuffer(FramebufferResource* frame_buffer,
                                     units::pixel width, units::pixel height,
                                     std::vector<TextureHint> formats, bool store_depth)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a TextureResource and binds it, combined with the supplied
    /// PixelData, to the graphics API permitting their use for rendering calls.
    ///
    /// \param texture TextureResource for identifying PixelData in future calls
    /// \param pixel_data Raw bitmap and format settings of the texture
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool RegisterTexture(TextureResource* texture, PixelData* pixel_data)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a ShaderResource, shader source files, and list of attributes,
    /// compiling them all into a single shader program to be used for rendering
    /// meshes each frame.
    ///
    /// \param program ShaderResource for identifying shader code in future calls
    /// \param vertex_filename Filename of the vertex shader source
    /// \param pixel_filename Filename of the pixel (fragment) shader source
    /// \param inputs List of attributes to be passed into the shader on each draw
    /// call. Allows for translation of vertex data to shader data
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool RegisterShader(ShaderResource* program,
                                std::string vertex_filename, std::string pixel_filename,
                                ShaderAttributeList inputs)=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Binds the supplied ShaderResource and renders a number of vertices
    /// from the currently bound mesh equal to the supplied index_count. Currently
    /// bound mesh is determined by the most recent call to Render::BindMeshBuffer
    ///
    /// \param program %Shader pipeline to render with
    /// \param index_count Number of vertices to render
    ////////////////////////////////////////////////////////////////////////////////
    virtual void RenderShader(ShaderResource* program, unsigned int index_count)=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a FramebufferResource and binds it to the graphics API,
    /// prepping it for a draw call. Passing a value of nullptr will bind the
    /// back buffer as the current framebuffer. Updates the viewport to match the
    /// texture size of the buffer
    ///
    /// \param frame_buffer Frame buffer resource
    ////////////////////////////////////////////////////////////////////////////////
    virtual void BindFramebuffer(FramebufferResource* frame_buffer)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a list of all the render targets bound to a frame buffer
    ///
    /// \return Ordered list of TextureResources representing each render target
    ////////////////////////////////////////////////////////////////////////////////
    virtual std::vector<const TextureResource*> FramebufferTextures(FramebufferResource* frame_buffer)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a pointer to the depth texture bound to a frame buffer
    ///
    /// \return TextureResource representing the depth buffer
    ////////////////////////////////////////////////////////////////////////////////
    virtual const TextureResource* FramebufferDepthTexture(FramebufferResource* frame_buffer)=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a supplied vertex and index buffer resource and binds them
    /// to the graphics API, prepping them for a draw call
    ///
    /// \param vertex_buffer Buffer pointing to renderable vertex data
    /// \param index_buffer Buffer pointing to the indices of vertex data
    ////////////////////////////////////////////////////////////////////////////////
    virtual void BindMeshBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the mesh data of the supplied vertex and index buffer to match
    /// that of the provided vertices and indices. Resizes the buffers if needed
    ///
    /// \param vertex_buffer Buffer pointing to renderable vertex data
    /// \param index_buffer Buffer pointing to the indices of vertex data
    /// \param vertices %Vertex data to be bound to the buffer
    /// \param vert_count Number of vertices to bind to the buffer
    /// \param indices Index data to be bound to the buffer
    /// \param index_count Number of indices to bind to the buffer
    ////////////////////////////////////////////////////////////////////////////////
    virtual void SetMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                             const Vertex* vertices, unsigned int vert_count,
                             const unsigned int* indices, unsigned int index_count)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Updates a subset of the mesh data bound to the supplied vertex and
    /// index buffer to match that of the provided vertices and indices. The offsets
    /// point to the beginning of the region to modify and are counted in number of
    /// vertices/indices. The end of the region is determined by offset + count
    ///
    /// \param vertex_buffer Buffer pointing to renderable vertex data
    /// \param index_buffer Buffer pointing to the indices of vertex data
    /// \param vertices %Vertex data to be bound to the buffer
    /// \param vert_offset Offset, in vertices, pointing to the beginning of the
    /// subregion to modify
    /// \param vert_count Number of vertices to update in the buffer
    /// \param indices Index data to be bound to the buffer
    /// \param index_offset Offset, in 4 byte chunks, pointing to the beginning of
    /// the subregion to modify
    /// \param index_count Number of indices to update in the buffer
    ////////////////////////////////////////////////////////////////////////////////
    virtual void UpdateMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                const Vertex* vertices, unsigned int vert_offset, unsigned int vert_count,
                                const unsigned int* indices, unsigned int index_offset, unsigned int index_count)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a vertex and index buffer and returns, through the input
    /// arguments, pointers to the mesh data stored internally in the rendering API
    /// allowing for free modification of the data. **Note that the pointers
    /// returned by this function are only valid until the next call to this
    /// class.** I wanted to make this more explicit through API usage somehow, but
    /// safe guards like std::weak_ptr%s and lambda callbacks gave too much of a
    /// performance hit. Sorry!
    ///
    /// \param vertex_buffer Buffer pointing to vertex data to modify
    /// \param index_buffer Buffer pointing to the indices to modify
    /// \param[out] vertex_data Pointer to the internally stored vertices
    /// \param[out] index_data Pointer to the internally stored indices
    ////////////////////////////////////////////////////////////////////////////////
    virtual void MapMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                             Vertex** vertex_data, unsigned int** index_data)=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a shader's global variable to be that of the given value
    ///
    /// \param program Shader containing global variable
    /// \param name Name of global variable to modify
    /// \param value Value to set global variable to
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const float value)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const int value)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const Matrix value)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const Vector2 value)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const Vector3 value)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const Vector4 value)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput
    ///
    /// \param texture_index The slot to bind the texture to
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const TextureResource* value, unsigned int texture_index)=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the blending mode for overwritten fragments. Defaults to
    /// BlendMode::ALPHA
    ///
    /// \param mode The blend mode to be set to
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetBlendMode(BlendMode mode)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the backface culling mode for vertex draw calls. Defaults to
    /// CullMode::ENABLE_CCW
    ///
    /// \param mode The cull mode to be set to
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetCullMode(CullMode mode)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets depth testing to be either enabled or disabled
    ///
    /// \param enable True to enable depth testing, false to disable
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetDepthTesting(bool enable)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the region of the buffer to render to during drawcalls
    ///
    /// \param viewport Box defining the render region
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetViewport(Box viewport)=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the video card information
    ///
    /// \param[out] buffer Pointer to the string containing information
    /// \param[out] len_buffer Number of bytes copied into buffer
    ////////////////////////////////////////////////////////////////////////////////
    virtual void VideoCardInfo(char* buffer, int& len_buffer)=0;

    // TODO: do this MANULLY in texture class later
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Decodes an image into raw pixel data and format information. This is
    /// only **temporarily** handled by the render class because the image decoding
    /// library also handles API specific functions
    ///
    /// \param filename Name of the image file to load
    /// \param[out] pixel_data Stores decoded pixel data and format information
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool LoadPixelData(std::string filename, PixelData* pixel_data)=0;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief True is vsync should be enabled
    ////////////////////////////////////////////////////////////////////////////////
    bool vsync_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Video card memory in bytes
    ////////////////////////////////////////////////////////////////////////////////
    int video_card_memory_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Vendor information of video card
    ////////////////////////////////////////////////////////////////////////////////
    std::string video_card_desc_;
};
} // namespace blons

// TODO: Add wayyyy more examples of dealing with the Render API at a low level
////////////////////////////////////////////////////////////////////////////////
/// \class blons::Render
/// \ingroup graphics
///
/// Provides a platform independent way of rendering raw meshes and textures
/// to the screen with a programmable pipeline.
/// 
/// ### Example:
/// \code
/// // Simple render loop dealing with a RenderContext
/// bool RenderScene()
/// {
///     // Clear buffers
///     context->BeginScene();
///
///     // Update camera matrix
///     camera->Render();
///
///     // Get view matrix
///     view_matrix = camera->view_matrix();
///
///     // Prep the pipeline for drawing
///     model->Render(context);
///     world_matrix = model->world_matrix();
///
///     // Set the inputs
///     if (!shader->SetInput("world_matrix", world_matrix, context) ||
///         !shader->SetInput("view_matrix", view_matrix, context) ||
///         !shader->SetInput("proj_matrix", proj_matrix, context) ||
///         !shader->SetInput("albedo", model->texture(), context))
///     {
///         return false;
///     }
///
///     // Finally do the render
///     if (!shader->Render(model->index_count(), context))
///     {
///         return false;
///     }
///
///     // Swap buffers
///     context->EndScene();
///
///     return true;
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_RENDER_RENDER_H_
