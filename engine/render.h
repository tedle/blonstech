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
    std::unique_ptr<unsigned char[]> pixels;
    int width;
    int height;
    enum BitDepth {
        A8       = 8,
        R8G8B8   = 24,
        R8G8B8A8 = 32
    } bits;
    enum Format {
        AUTO, // Compresses to DXT5 & generates mipmaps
        DDS,  // Uses mipmaps & compression from image file
        RAW   // Will not gen mipmaps, will not compress on GPU
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

    virtual BufferResource* CreateBufferResource()=0;
    virtual TextureResource* CreateTextureResource()=0;
    virtual ShaderResource* CreateShaderResource()=0;

    virtual bool RegisterMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                              Vertex* vertices, unsigned int vert_count,
                              unsigned int* indices, unsigned int index_count)=0;
    virtual bool RegisterQuad(BufferResource* vertex_buffer, BufferResource* index_buffer)=0;
    virtual bool RegisterTexture(TextureResource* texture, PixelData* pixel_data)=0;
    virtual bool RegisterShader(ShaderResource* program,
                                const char* vertex_filename, const char* pixel_filename,
                                ShaderAttributeList inputs)=0;

    virtual void RenderShader(ShaderResource* program, int index_count)=0;

    virtual void SetModelBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)=0;
    virtual bool SetShaderInput(ShaderResource* program, const char* name, Matrix value)=0;
    virtual bool SetShaderInput(ShaderResource* program, const char* name, TextureResource* value)=0;

    virtual Matrix projection_matrix()=0;
    virtual Matrix ortho_matrix()=0;

    virtual void GetVideoCardInfo(char* buffer, int& len_buffer)=0;

    // TODO: do this MANULLY in texture class later
    virtual bool LoadPixelData(const char* filename, PixelData* pixel_data)=0;

protected:
    bool vsync_;
    int video_card_memory_;
    std::string video_card_desc_;
    Matrix proj_matrix_;
    Matrix ortho_matrix_;
};
} // namespace blons

#endif
