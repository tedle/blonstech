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

#ifndef BLONSTECH_GRAPHICS_RENDER_RENDERER_H_
#define BLONSTECH_GRAPHICS_RENDER_RENDERER_H_

// Includes
#include <memory>
#include <string>
#include <vector>
// Public Includes
#include <blons/graphics/render/context.h>
#include <blons/math/math.h>
#include <blons/debug/console.h>
#include <blons/debug/log.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains formatting information for creating new textures
////////////////////////////////////////////////////////////////////////////////
struct TextureType
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines the bit format of a texture
    ////////////////////////////////////////////////////////////////////////////////
    enum Format
    {
        NONE,               ///< Error
        A8,                 ///< 1-channel, 8-bit per channel
        R8G8_UINT,          ///< 2-channel, 8-bit per channel
        R8G8B8_UINT,        ///< 3-channel, 8-bit per channel
        R8G8B8A8_UINT,      ///< 4-channel, 8-bit per channel
        R8G8B8,             ///< 3-channel, 8-bit per channel
        R16G16_UNORM,       ///< 2-channel, 16-bit per channel
        R16G16B16_UNORM,    ///< 3-channel, 16-bit per channel
        R16G16B16A16_UNORM, ///< 4-channel, 16-bit per channel
        R16G16_FLOAT,       ///< 2-channel, 16-bit per channel
        R16G16B16_FLOAT,    ///< 3-channel, 16-bit per channel
        R16G16B16A16_FLOAT, ///< 4-channel, 16-bit per channel
        R8G8B8A8,           ///< 3-channel, 8-bit per channel
        A32,                ///< 1-channel, 32-bit per channel
        R32G32,             ///< 2-channel, 32-bit per channel
        R32G32B32,          ///< 3-channel, 32-bit per channel
        R32G32B32A32,       ///< 4-channel, 32-bit per channel
        DEPTH               ///< 24-bit Depth information
    } format; ///< \copybrief Format

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines the compression format of a texture
    ////////////////////////////////////////////////////////////////////////////////
    enum Compression
    {
        AUTO,      ///< Generates mipmaps
        DDS,       ///< Uses mipmaps & compression from image file
        RAW        ///< Will not generate mipmaps, will not compress on GPU
    } compression; ///< \copybrief Compression

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines the filtering for texture access
    ////////////////////////////////////////////////////////////////////////////////
    enum Filter
    {
        NEAREST, ///< Nearest neighbour filtering
        LINEAR   ///< Linear filtering
    } filter; ///< \copybrief Filter

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines the UV wrapping behaviour for texture access
    ////////////////////////////////////////////////////////////////////////////////
    enum Wrap
    {
        CLAMP, ///< Samples edge pixel when out of bounds
        REPEAT ///< Tiled repetition
    } wrap; ///< \copybrief Wrap

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Contains texture storage and rendering options
    ////////////////////////////////////////////////////////////////////////////////
    struct Options
    {
        Compression compression; ///< \copybrief TextureType::Compression
        Filter filter;           ///< \copybrief TextureType::Filter
        Wrap wrap;               ///< \copybrief TextureType::Wrap
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes TextureType with all settings specified
    ////////////////////////////////////////////////////////////////////////////////
    TextureType(Format format, Compression compression, Filter filter, Wrap wrap) : format(format), compression(compression), filter(filter), wrap(wrap) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes TextureType with a default setting of
    /// `TextureType::Wrap::REPEAT`
    ////////////////////////////////////////////////////////////////////////////////
    TextureType(Format format, Compression compression, Filter filter) : TextureType(format, compression, filter, REPEAT) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes TextureType with a default setting of
    /// `TextureType::Compression::RAW`
    ////////////////////////////////////////////////////////////////////////////////
    TextureType(Format format, Filter filter, Wrap wrap) : TextureType(format, RAW, filter, wrap) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes TextureType with default settings of
    /// `TextureType::Filter::NEAREST`, `TextureType::Wrap::REPEAT`
    ////////////////////////////////////////////////////////////////////////////////
    TextureType(Format format, Compression compression) : TextureType(format, compression, NEAREST, REPEAT) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes TextureType with default settings of
    /// `TextureType::Compression::RAW`, `TextureType::Wrap::REPEAT`
    ////////////////////////////////////////////////////////////////////////////////
    TextureType(Format format, Filter filter) : TextureType(format, RAW, filter, REPEAT) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes TextureType with default settings of
    /// `TextureType::Compression::RAW`, `TextureType::Filter::NEAREST`,
    /// `TextureType::Wrap::REPEAT`
    ////////////////////////////////////////////////////////////////////////////////
    TextureType(Format format) : TextureType(format, RAW, NEAREST, REPEAT) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes TextureType with default settings of
    /// `TextureType::Format::NONE`, `TextureType::Compression::RAW`,
    /// `TextureType::Filter::NEAREST`, `TextureType::Wrap::REPEAT`
    ////////////////////////////////////////////////////////////////////////////////
    TextureType() : TextureType(NONE, RAW, NEAREST, REPEAT) {}
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Used to set the blending mode for future render passes
////////////////////////////////////////////////////////////////////////////////
enum BlendMode
{
    ADDITIVE, ///< Pixel shader value is added to old value
    ALPHA,    ///< Pixel shader value is mixed with old value based on alpha
    OVERWRITE ///< Pixel shader value replaces value on buffer
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Used to set the culling mode for future render passes
////////////////////////////////////////////////////////////////////////////////
enum CullMode
{
    ENABLE_CCW, ///< Only show triangles with counter-clockwise winding
    ENABLE_CW,  ///< Only show triangle with clockwise winding
    DISABLE     ///< Disable culling of triangles based on winding
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Used to set how vertices are interpreted as primitives in future draw
/// calls
////////////////////////////////////////////////////////////////////////////////
enum DrawMode
{
    LINES,    ///< Draw vertices in groups of two as lines
    TRIANGLES ///< Draw vertices in groups of three as triangles
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
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines how to structure vertices into primitives
    ////////////////////////////////////////////////////////////////////////////////
    DrawMode draw_mode;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Holds raw pixel data and format info of a texture. Can also hold
/// compressed pixel data for direct uploads, in which case width and height are
/// ignored during GPU upload and bits_per_pixel() becomes non-functional.
////////////////////////////////////////////////////////////////////////////////
struct PixelData
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Raw pixel data
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<unsigned char> pixels;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Width of the texture in pixels
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel width;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Height of the texture in pixels
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel height;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines the texture storage and behaviour options
    ////////////////////////////////////////////////////////////////////////////////
    TextureType type;

    PixelData() {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs a full copy of the given PixelData, including a byte for
    /// byte copy of the pixel buffer
    ////////////////////////////////////////////////////////////////////////////////
    PixelData(const PixelData& p)
    {
        this->width = p.width;
        this->height = p.height;
        this->type = p.type;
        this->pixels = p.pixels;
    }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor that takes pixel buffer without copying
    ////////////////////////////////////////////////////////////////////////////////
    PixelData(PixelData&& p)
    {
        this->width = std::move(p.width);
        this->height = std::move(p.height);
        this->type = std::move(p.type);
        this->pixels = std::move(p.pixels);
    }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the amount of bits per pixel. For 8-bit/channel formats
    /// data is stored as one unsigned char per channel. Anything greater is stored
    /// one float per channel. Non-functional if pixel data
    /// is compressed
    ///
    /// \return Bits per pixel
    ////////////////////////////////////////////////////////////////////////////////
    std::size_t bits_per_pixel() const
    {
        if (type.compression == TextureType::DDS)
        {
            throw "Bits per pixel cannot be calculated on compressed pixel data";
        }
        int bits;
        switch (type.format)
        {
        case TextureType::A8:
            bits = 8;
            break;
        case TextureType::R8G8_UINT:
            bits = 16;
            break;
        case TextureType::R8G8B8:
            bits = 24;
            break;
        case TextureType::A32:
        case TextureType::R8G8B8A8:
        case TextureType::DEPTH:
            bits = 32;
            break;
        case TextureType::R16G16_FLOAT:
        case TextureType::R16G16_UNORM:
        case TextureType::R32G32:
            bits = 64;
            break;
        case TextureType::R32G32B32:
            bits = 96;
            break;
        case TextureType::R32G32B32A32:
            bits = 128;
            break;
        case TextureType::NONE:
        default:
            assert(false);
            break;
        }
        return bits;
    }
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Holds raw pixel data and format info of a 3D texture
////////////////////////////////////////////////////////////////////////////////
struct PixelData3D : public PixelData
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Depth of the texture in pixels
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel depth;

    PixelData3D() {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs a full copy of the given PixelData3D, including a byte for
    /// byte copy of the pixel buffer
    ////////////////////////////////////////////////////////////////////////////////
    PixelData3D(const PixelData3D& p)
    {
        this->width = p.width;
        this->height = p.height;
        this->depth = p.depth;
        this->type = p.type;
        this->pixels = p.pixels;
    }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor that takes pixel buffer without copying
    ////////////////////////////////////////////////////////////////////////////////
    PixelData3D(PixelData3D&& p)
    {
        this->width = std::move(p.width);
        this->height = std::move(p.height);
        this->depth = std::move(p.depth);
        this->type = std::move(p.type);
        this->pixels = std::move(p.pixels);
    }
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Holds raw pixel data and format info of a cubemap texture
////////////////////////////////////////////////////////////////////////////////
struct PixelDataCubemap : public PixelData
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Raw pixel data partitioned into 6 faces, indexed by their
    /// blons::AxisAlignedNormal equivilent
    ////////////////////////////////////////////////////////////////////////////////
    std::array<decltype(PixelData::pixels), 6> pixels;
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
using ShaderAttribute = std::pair<ShaderAttributeIndex, std::string>;
////////////////////////////////////////////////////////////////////////////////
/// \brief Holds a list of shader attributes
////////////////////////////////////////////////////////////////////////////////
using ShaderAttributeList = std::vector<ShaderAttribute>;
////////////////////////////////////////////////////////////////////////////////
/// \brief List of valid shader pipeline stages
////////////////////////////////////////////////////////////////////////////////
enum ShaderPipelineStage
{
    VERTEX,   ///< Vertex shader
    PIXEL,    ///< Pixel shader
    GEOMETRY, ///< Geometry shader
    COMPUTE   ///< Compute shader
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Holds a shader's type and source
////////////////////////////////////////////////////////////////////////////////
using ShaderSource = std::pair<ShaderPipelineStage, std::string>;
////////////////////////////////////////////////////////////////////////////////
/// \brief Holds a list of shader sources
////////////////////////////////////////////////////////////////////////////////
using ShaderSourceList = std::vector<ShaderSource>;

// Forward declarations
class BufferResource;
class FramebufferResource;
class ShaderResource;
class ShaderDataResource;
class TextureResource;
class TimerResource;
////////////////////////////////////////////////////////////////////////////////
/// \brief Class for interfacing with a graphics API
////////////////////////////////////////////////////////////////////////////////
class Renderer
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to identify different Renderer contexts
    ////////////////////////////////////////////////////////////////////////////////
    using ContextID = std::size_t;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Contains description of video card name and available memory
    ////////////////////////////////////////////////////////////////////////////////
    struct VideoCardInfo
    {
        std::string name; ///< Name describing the video card
        uint64_t memory;  ///< Max available memory in bytes
    };

public:
    Renderer();
    virtual ~Renderer() {};

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves unique identifier for context
    ///
    /// \return Unique ContextID
    ////////////////////////////////////////////////////////////////////////////////
    ContextID id();

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
    /// \brief Takes a list of vertices and indices, binding them to the graphics
    /// API and permitting their use for rendering calls. Sets up shader inputs
    /// using the entirety of supplied vertex data. Returns a BufferResource with
    /// the bound data
    ///
    /// \param vertices Vertices to be bound to buffer, may be nullptr if vert_count
    /// and index_count are 0
    /// \param vert_count Number of vertices to be bound to buffer, may be 0 for an
    /// empty mesh
    /// \param indices Indices to be bound to buffer, may be nullptr if vert_count
    /// and index_count are 0
    /// \param index_count Number of indices to be bound to buffer, may be 0 for an
    /// empty mesh
    /// \param draw_mode Describes how to form primitives from vertices
    /// \return BufferResource containing mesh data, or nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////
    virtual BufferResource* RegisterMesh(Vertex* vertices, unsigned int vert_count,
                                         unsigned int* indices, unsigned int index_count,
                                         DrawMode draw_mode)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Generates a FramebufferResource bound to the graphics API permitting
    /// its use for rendering calls. Expects an output to the supplied number of
    /// render targets.
    ///
    /// \param width Width of the textures to render to in pixels
    /// \param height Height of the textures to render to in pixels
    /// \param formats List of formatted textures to output to
    /// \param store_depth Store the depth buffer from each render to a texture
    /// \return FramebufferResource on success, nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////
    virtual FramebufferResource* RegisterFramebuffer(units::pixel width, units::pixel height,
                                                     std::vector<TextureType> formats, bool store_depth)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a provided pixel buffer and generates a TextureResource bound
    /// to the graphics API permitting their use for rendering calls.
    ///
    /// \param pixel_data Raw bitmap and format settings of the texture
    /// \return TextureResource on success, nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////
    virtual TextureResource* RegisterTexture(PixelData* pixel_data)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc RegisterTexture(PixelData*)
    ////////////////////////////////////////////////////////////////////////////////
    virtual TextureResource* RegisterTexture(PixelData3D* pixel_data)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc RegisterTexture(PixelData*)
    ////////////////////////////////////////////////////////////////////////////////
    virtual TextureResource* RegisterTexture(PixelDataCubemap* pixel_data)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a list of shader source files and of attributes, compiling them
    /// all into a single shader program to be used for rendering meshes each frame.
    ///
    /// \param source Source code for the shaders
    /// \param inputs List of attributes to be passed into the shader on each draw
    /// call. Allows for translation of vertex data to shader data
    /// \return ShaderResource on success, nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////
    virtual ShaderResource* RegisterShader(ShaderSourceList source, ShaderAttributeList inputs)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a list of source files, compiling it into a compute shader that
    /// can be run at any time.
    ///
    /// \param source Source code for the compute shader
    /// \return ShaderResource on success, nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////
    virtual ShaderResource* RegisterComputeShader(ShaderSourceList source)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Generates a ShaderDataResource from arbitrary memory, creating a
    /// bindable block allowing easy communication between CPU & GPU memory.
    ///
    /// \param data Memory to be copied. May be nullptr for unitialized memory
    /// \param size Size of memory block to allocate. Immutable
    /// \return ShaderDataResource on success, nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////
    virtual ShaderDataResource* RegisterShaderData(const void* data, std::size_t size)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Marks a point in the GPU command buffer to query the current internal
    /// clock. Returns a TimerResource handle to later retrieve the timestamp once
    /// the preceding commands have finished running.
    ///
    /// \return TimerResource on success, nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////
    virtual TimerResource* RegisterTimestamp()=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Binds the supplied ShaderResource and renders a number of vertices
    /// from the currently bound mesh equal to the supplied index_count. Currently
    /// bound mesh is determined by the most recent call to Renderer::BindMeshBuffer
    ///
    /// \param program %Shader pipeline to render with
    /// \param index_count Number of vertices to render
    ////////////////////////////////////////////////////////////////////////////////
    virtual void RenderShader(ShaderResource* program, unsigned int index_count)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc RenderShader(ShaderResource*,unsigned int)
    /// \param instance_count Number of instances to render
    ////////////////////////////////////////////////////////////////////////////////
    virtual void RenderShaderInstanced(ShaderResource* program, unsigned int index_count, unsigned int instance_count)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Binds and runs the supplied ShaderResource with the specified thread
    /// group dimensions. Further granularity of threads can usually be described at
    /// the shader level and in this case the total number of threads will be
    /// `(groups_x * shader_x) * (groups_y * shader_y) * (groups_z * shader_z)`
    ///
    /// \param program %ComputeShader to render with
    /// \param groups_x Number of thread groups running on the X axis
    /// \param groups_y Number of thread groups running on the Y axis
    /// \param groups_z Number of thread groups running on the Z axis
    ////////////////////////////////////////////////////////////////////////////////
    virtual void RunComputeShader(ShaderResource* program, unsigned int groups_x,
                                  unsigned int groups_y, unsigned int groups_z)=0;

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
    /// \brief Takes a FramebufferResource and attaches a provided list of colour
    /// textures to it, overriding any previously bound targets.
    ///
    /// \param frame_buffer Frame buffer resource
    /// \param colour_textures List of output texture resources
    /// \param mip_level Mipmap level of the textures to be bound, with 0 being the
    /// base
    ////////////////////////////////////////////////////////////////////////////////
    virtual void SetFramebufferColourTextures(FramebufferResource* frame_buffer, const std::vector<const TextureResource*>& colour_textures, unsigned int mip_level)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a FramebufferResource and attaches a provided depth texture to
    /// it. If `depth_texture` is null, the current depth buffer will be unbound
    /// from the framebuffer
    ///
    /// \param frame_buffer Frame buffer resource
    /// \param depth_texture Texture resource of depth texture
    /// \param mip_level Mipmap level of the texture to be bound, with 0 being the
    /// base
    ////////////////////////////////////////////////////////////////////////////////
    virtual void SetFramebufferDepthTexture(FramebufferResource* frame_buffer, const TextureResource* depth_texture, unsigned int mip_level)=0;
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
    /// \brief Takes a supplied BufferResource and binds it to the graphics API,
    /// prepping it for a draw call
    ///
    /// \param buffer Buffer pointing to renderable data
    ////////////////////////////////////////////////////////////////////////////////
    virtual void BindMeshBuffer(BufferResource* buffer)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the mesh data of the supplied BufferResource to match that of
    /// the provided vertices and indices. Resizes the buffers if needed
    ///
    /// \param buffer Buffer pointing to renderable data
    /// \param vertices %Vertex data to be bound to the buffer
    /// \param vert_count Number of vertices to bind to the buffer
    /// \param indices Index data to be bound to the buffer
    /// \param index_count Number of indices to bind to the buffer
    ////////////////////////////////////////////////////////////////////////////////
    virtual void SetMeshData(BufferResource* buffer,
                             const Vertex* vertices, unsigned int vert_count,
                             const unsigned int* indices, unsigned int index_count)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Updates a subset of the mesh data bound to the supplied buffer to
    /// match that of the provided vertices and indices. The offsets point to the
    /// beginning of the region to modify and are counted in number of
    /// vertices/indices. The end of the region is determined by offset + count
    ///
    /// \param buffer Buffer pointing to renderable data
    /// \param vertices %Vertex data to be bound to the buffer
    /// \param vert_offset Offset, in vertices, pointing to the beginning of the
    /// subregion to modify
    /// \param vert_count Number of vertices to update in the buffer
    /// \param indices Index data to be bound to the buffer
    /// \param index_offset Offset, in 4 byte chunks, pointing to the beginning of
    /// the subregion to modify
    /// \param index_count Number of indices to update in the buffer
    ////////////////////////////////////////////////////////////////////////////////
    virtual void UpdateMeshData(BufferResource* buffer,
                                const Vertex* vertices, unsigned int vert_offset, unsigned int vert_count,
                                const unsigned int* indices, unsigned int index_offset, unsigned int index_count)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a buffer and returns, through the input arguments, pointers to
    /// the mesh data stored internally in the rendering API allowing for free
    /// modification of the data. **Note that the pointers returned by this function
    /// are only valid until the next call to this class.** I wanted to make this
    /// more explicit through API usage somehow, but safe guards like
    /// std::weak_ptr%s and lambda callbacks gave too much of a performance hit.
    /// Sorry!
    ///
    /// \param buffer Buffer pointing to data to modify
    /// \param[out] vertex_data Pointer to the internally stored vertices
    /// \param[out] index_data Pointer to the internally stored indices
    ////////////////////////////////////////////////////////////////////////////////
    virtual void MapMeshData(BufferResource* buffer,
                             Vertex** vertex_data, unsigned int** index_data)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the pixel data and formatting of the supplied texture resource.
    /// Overwrites old texture data completely, including width/height, format
    /// settings, and compression. For TextureType::AUTO textures, mipmaps are
    /// completely reconstructed for each call that modifies the base level image
    ///
    /// \param texture Texture to be set
    /// \param pixels Pixel data to update texture with
    /// \param mip_level Mipmap level to set data for, with 0 being the base
    ////////////////////////////////////////////////////////////////////////////////
    virtual void SetTextureData(TextureResource* texture, PixelData* pixels, unsigned int mip_level)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetTextureData(TextureResource*, PixelData*, unsigned int)
    ////////////////////////////////////////////////////////////////////////////////
    virtual void SetTextureData(TextureResource* texture, PixelData3D* pixels, unsigned int mip_level)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetTextureData(TextureResource*, PixelData*, unsigned int)
    ////////////////////////////////////////////////////////////////////////////////
    virtual void SetTextureData(TextureResource* texture, PixelDataCubemap* pixels, unsigned int mip_level)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves pixel data and formatting of a supplied texture from the
    /// GPU
    ///
    /// \param texture Texture to fetch
    /// \return Pixel data and information
    /// \param mip_level Mipmap level to get data of, with 0 being the base
    ////////////////////////////////////////////////////////////////////////////////
    virtual PixelData GetTextureData(const TextureResource* texture, unsigned int mip_level)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc GetTextureData(const TextureResource*, unsigned int)
    ////////////////////////////////////////////////////////////////////////////////
    virtual PixelData3D GetTextureData3D(const TextureResource* texture, unsigned int mip_level)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc GetTextureData(const TextureResource*, unsigned int)
    ////////////////////////////////////////////////////////////////////////////////
    virtual PixelDataCubemap GetTextureDataCubemap(const TextureResource* texture, unsigned int mip_level)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Makes mipmaps for a texture from the base image layer.
    ///
    /// \param texture Texture to make mipmaps for
    ////////////////////////////////////////////////////////////////////////////////
    virtual void MakeTextureMipmaps(TextureResource* texture)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the range of mipmap levels accessible by the texture.
    ///
    /// \param texture Texture to clamp mipmap range of
    /// \param min_level Upper level of detail, highest being 0
    /// \param max_level Lower level of detail, highest being 0
    ////////////////////////////////////////////////////////////////////////////////
    virtual void SetTextureMipmapRange(TextureResource* texture, int min_level, int max_level)=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets arbitrary data of shader memory blocks. Size is immutable and
    /// determined at ShaderDataResource creation time
    ///
    /// \param data_handle Storage block handle
    /// \param offset The offset at to which data should be copied to in bytes
    /// \param length The length of data that should be copied in bytes
    /// \param[in] data Memory block to copy data from
    ////////////////////////////////////////////////////////////////////////////////
    virtual void SetShaderData(ShaderDataResource* data_handle, std::size_t offset, std::size_t length, const void* data)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves arbitrary data from shader memory blocks. Size is immutable
    /// and determined at ShaderDataResource creation time
    ///
    /// \param data_handle Storage block handle
    /// \param[out] data Allocated memory block to copy data into
    ////////////////////////////////////////////////////////////////////////////////
    virtual void GetShaderData(ShaderDataResource* data_handle, void* data)=0;

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
    /// \copydoc SetShaderInput
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const ShaderDataResource* value)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a shader's global array to be that of the given value
    ///
    /// \param program Shader containing global array
    /// \param name Name of global array to modify
    /// \param value Pointer to an array of values
    /// \param elements Number of elements in array
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const float* value, std::size_t elements)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput(ShaderResource*, const char*, const float*, std::size_t)
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const int* value, std::size_t elements)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput(ShaderResource*, const char*, const float*, std::size_t)
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const Matrix* value, std::size_t elements)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput(ShaderResource*, const char*, const float*, std::size_t)
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const Vector2* value, std::size_t elements)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput(ShaderResource*, const char*, const float*, std::size_t)
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const Vector3* value, std::size_t elements)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetShaderInput(ShaderResource*, const char*, const float*, std::size_t)
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const Vector4* value, std::size_t elements)=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a shader's global output variable to be that of the given value
    ///
    /// \param program Shader containing output variable
    /// \param name Name of output variable to modify
    /// \param value Value to set output variable to
    /// \param texture_index The slot to bind the texture to
    /// \param mip_level Mipmap level of the texture to bind
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetShaderOutput(ShaderResource* program, const char* name, TextureResource* value, unsigned int texture_index, unsigned int mip_level)=0;

    // TODO: Replace with std::optional when we have vs2017
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Queries the GPU to see if the provided TimerResource is valid, and if
    /// so will return the marked time in microseconds. Returns 0 if the GPU command
    /// buffer has not yet reached the point where the timestamp should be generated
    ///
    /// \param timestamp TimerResource containing timestamp marker
    /// \return Marked time in microseconds on success, 0 on failure
    ////////////////////////////////////////////////////////////////////////////////
    virtual units::time::us GetTimestamp(TimerResource* timestamp)=0;

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
    /// \param x Left-most edge of the viewport
    /// \param y Upper-most edge of the viewport
    /// \param width Width of the viewport
    /// \param height Height of the viewport
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool SetViewport(units::pixel x, units::pixel y, units::pixel width, units::pixel height)=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the video card information
    ///
    /// \return Struct containing video card information
    ////////////////////////////////////////////////////////////////////////////////
    virtual VideoCardInfo video_card_info()=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the depth buffer range of the implemented rendering API
    ///
    /// \return True if the depth buffer ranges from [0,1], false if range is [-1,1]
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool IsDepthBufferRangeZeroToOne() const=0;

    // TODO: do this MANULLY in texture class later
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Decodes an image into raw pixel data and format information. This is
    /// only **temporarily** handled by the render class because the image decoding
    /// library also handles API specific functions. When loading compressed images
    /// a raw buffer of the compressed data is instead stored, as well width and
    /// height are set to 0.
    ///
    /// \param filename Name of the image file to load
    /// \param[out] pixel_data Stores decoded pixel data and format information
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool LoadPixelData(std::string filename, PixelData* pixel_data)=0;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Unique identifier for each Renderer context
    ////////////////////////////////////////////////////////////////////////////////
    const ContextID id_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief True is vsync should be enabled
    ////////////////////////////////////////////////////////////////////////////////
    bool vsync_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Struct containing video card name and available memory
    ////////////////////////////////////////////////////////////////////////////////
    VideoCardInfo video_card_info_;

private:
    static ContextID context_count;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Contains identifying data to render data to the screen
////////////////////////////////////////////////////////////////////////////////
class BufferResource
{
public:
    virtual ~BufferResource() {};
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines the context owning this resource
    ////////////////////////////////////////////////////////////////////////////////
    const Renderer::ContextID context_id;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes resource with parent context ID
    ///
    /// \param parent_id Parent context ID
    ////////////////////////////////////////////////////////////////////////////////
    BufferResource(Renderer::ContextID parent_id) : context_id(parent_id) {}
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains identifying data to render data to various textures
////////////////////////////////////////////////////////////////////////////////
class FramebufferResource
{
public:
    virtual ~FramebufferResource() {};
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc BufferResource::context_id
    ////////////////////////////////////////////////////////////////////////////////
    const Renderer::ContextID context_id;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc BufferResource::BufferResource(Renderer::ContextID)
    ////////////////////////////////////////////////////////////////////////////////
    FramebufferResource(Renderer::ContextID parent_id) : context_id(parent_id) {}
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains identifying data to activate and ineract with shaders
////////////////////////////////////////////////////////////////////////////////
class ShaderResource
{
public:
    virtual ~ShaderResource() {};
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc BufferResource::context_id
    ////////////////////////////////////////////////////////////////////////////////
    const Renderer::ContextID context_id;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc BufferResource::BufferResource(Renderer::ContextID)
    ////////////////////////////////////////////////////////////////////////////////
    ShaderResource(Renderer::ContextID parent_id) : context_id(parent_id) {}
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains identifying data to ineract with shader memory
////////////////////////////////////////////////////////////////////////////////
class ShaderDataResource
{
public:
    virtual ~ShaderDataResource() {};
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc BufferResource::context_id
    ////////////////////////////////////////////////////////////////////////////////
    const Renderer::ContextID context_id;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc BufferResource::BufferResource(Renderer::ContextID)
    ////////////////////////////////////////////////////////////////////////////////
    ShaderDataResource(Renderer::ContextID parent_id) : context_id(parent_id) {}
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains identifying data to bind textures
////////////////////////////////////////////////////////////////////////////////
class TextureResource
{
public:
    virtual ~TextureResource() {};
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc BufferResource::context_id
    ////////////////////////////////////////////////////////////////////////////////
    const Renderer::ContextID context_id;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc BufferResource::BufferResource(Renderer::ContextID)
    ////////////////////////////////////////////////////////////////////////////////
    TextureResource(Renderer::ContextID parent_id) : context_id(parent_id) {}
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains identifying data to query the GPU clock
////////////////////////////////////////////////////////////////////////////////
class TimerResource
{
public:
    virtual ~TimerResource() {};
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc BufferResource::context_id
    ////////////////////////////////////////////////////////////////////////////////
    const Renderer::ContextID context_id;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc BufferResource::BufferResource(Renderer::ContextID)
    ////////////////////////////////////////////////////////////////////////////////
    TimerResource(Renderer::ContextID parent_id) : context_id(parent_id) {}
};
} // namespace blons

// TODO: Add wayyyy more examples of dealing with the Render API at a low level
////////////////////////////////////////////////////////////////////////////////
/// \class blons::Renderer
/// \ingroup graphics
///
/// Provides a platform independent way of rendering raw meshes and textures
/// to the screen with a programmable pipeline.
/// 
/// ### Example:
/// \code
/// // Simple render loop dealing with a Renderer context
/// bool RenderScene()
/// {
///     auto context = blons::renderer::context();
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
///     model->Render();
///     world_matrix = model->world_matrix();
///
///     // Set the inputs
///     if (!shader->SetInput("world_matrix", world_matrix) ||
///         !shader->SetInput("view_matrix", view_matrix) ||
///         !shader->SetInput("proj_matrix", proj_matrix) ||
///         !shader->SetInput("albedo", model->texture()))
///     {
///         return false;
///     }
///
///     // Finally do the render
///     if (!shader->Render(model->index_count()))
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

#endif // BLONSTECH_GRAPHICS_RENDER_RENDERER_H_
