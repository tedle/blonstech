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
////////////////////////////////////////////////////////////////////////////////
/// \brief Holds position, texture coords, and the normal of a single vertex
////////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    Vector3 pos;
    Vector2 tex;
    Vector3 norm;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Makes vertices sortable allowing for efficient std::map lookups
    ////////////////////////////////////////////////////////////////////////////////
    bool operator< (const Vertex vert) const {return memcmp(this, &vert, sizeof(Vertex))>0;}
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
    /// \brief Determines the pixel format of a texture
    ////////////////////////////////////////////////////////////////////////////////
    enum BitDepth {
        A8       = 8,  ///< Monochrome 8-bit
        R8G8B8   = 24, ///< 24-bit full colour, no alpha
        R8G8B8A8 = 32  ///< 32-bit full colour with alpha
    } bits;            ///< \copybrief BitDepth

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines the format and rendering options of a texture
    ////////////////////////////////////////////////////////////////////////////////
    enum Format {
        AUTO, ///< Compresses to DXT5 & generates mipmaps
        DDS,  ///< Uses mipmaps & compression from image file
        RAW   ///< Will not generate mipmaps, will not compress on GPU, use nearest neighbour filtering
    } format; ///< \copybrief Format
};

class BufferResource {
public:
    virtual ~BufferResource() {};
};
class ShaderResource {
public:
    virtual ~ShaderResource() {};
};
class TextureResource {
public:
    virtual ~TextureResource() {};
};

typedef std::pair<unsigned int, std::string> ShaderAttribute;
typedef std::vector<ShaderAttribute> ShaderAttributeList;
typedef std::unique_ptr<class RenderAPI> RenderContext;

class RenderAPI {

public:
    virtual ~RenderAPI() {};

    virtual void BeginScene()=0;
    virtual void EndScene()=0;

    virtual BufferResource* MakeBufferResource()=0;
    virtual TextureResource* MakeTextureResource()=0;
    virtual ShaderResource* MakeShaderResource()=0;

    virtual bool Register3DMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                Vertex* vertices, unsigned int vert_count,
                                unsigned int* indices, unsigned int index_count)=0;
    virtual bool Register2DMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                Vertex* vertices, unsigned int vert_count,
                                unsigned int* indices, unsigned int index_count)=0;
    virtual bool RegisterTexture(TextureResource* texture, PixelData* pixel_data)=0;
    virtual bool RegisterShader(ShaderResource* program,
                                std::string vertex_filename, std::string pixel_filename,
                                ShaderAttributeList inputs)=0;

    virtual void RenderShader(ShaderResource* program, unsigned int index_count)=0;

    virtual void BindMeshBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)=0;
    virtual void SetMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                             const Vertex* vertices, unsigned int vert_count,
                             const unsigned int* indices, unsigned int index_count)=0;
    virtual void UpdateMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                const Vertex* vertices, unsigned int vert_offset, unsigned int vert_count,
                                const unsigned int* indices, unsigned int index_offset, unsigned int index_count)=0;
    // TODO: The pointers returned by this are only good until the next API call,
    // make this more clear to the user somehow! I want it to be explicit in the API,
    // instead of just casually mentioned deep in the docs somewhere
    // NOTE: Tried using weak_ptrs, lambda callbacks. Both hurt performance too much
    virtual void MapMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                             Vertex** vertex_data, unsigned int** index_data)=0;
    // Using const char* instead of std::string here is noticably faster
    virtual bool SetShaderInput(ShaderResource* program, const char* name, int value)=0;
    virtual bool SetShaderInput(ShaderResource* program, const char* name, Matrix value)=0;
    virtual bool SetShaderInput(ShaderResource* program, const char* name, Vector3 value)=0;
    virtual bool SetShaderInput(ShaderResource* program, const char* name, Vector4 value)=0;
    virtual bool SetShaderInput(ShaderResource* program, const char* name, const TextureResource* value)=0;

    virtual bool SetDepthTesting(bool enable)=0;

    virtual void GetVideoCardInfo(char* buffer, int& len_buffer)=0;

    // TODO: do this MANULLY in texture class later
    virtual bool LoadPixelData(std::string filename, PixelData* pixel_data)=0;

protected:
    bool vsync_;
    int video_card_memory_;
    std::string video_card_desc_;
};
} // namespace blons

#endif // BLONSTECH_GRAPHICS_RENDER_RENDER_H_
