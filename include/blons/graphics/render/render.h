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
struct Vertex
{
    Vector3 pos;
    Vector2 tex;
    Vector3 norm;
    // needed for efficient std::map lookups
    bool operator< (const Vertex vert) const {return memcmp(this, &vert, sizeof(Vertex))>0;}
};

struct PixelData
{
    std::unique_ptr<unsigned char> pixels;
    units::pixel width;
    units::pixel height;
    enum BitDepth {
        A8       = 8,
        R8G8B8   = 24,
        R8G8B8A8 = 32
    } bits;
    enum Format {
        AUTO, // Compresses to DXT5 & generates mipmaps
        DDS,  // Uses mipmaps & compression from image file
        RAW   // Will not gen mipmaps, will not compress on GPU, use nearest neighbour filtering
    } format;
};

struct MatrixBuffer
{
    Matrix world;
    Matrix view;
    Matrix projection;
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
    virtual void SetMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                             const Vertex* vertices, unsigned int vert_offset, unsigned int vert_count,
                             const unsigned int* indices, unsigned int index_offset, unsigned int index_count)=0;
    virtual void MapMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                             void** vertex_data, void** index_data)=0;
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

#endif
