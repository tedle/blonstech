#ifndef BLONSTECH_SPRITE_H_
#define BLONSTECH_SPRITE_H_

// Includes
#include <functional>
// Local Includes
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/render/render.h"

namespace blons
{
class Sprite
{
public:
    struct Box
    {
        float x, y, w, h;

        Box() {}
        Box(float _x, float _y, float _w, float _h) : x(_x), y(_y), w(_w), h(_h) {}
    };

public:
    Sprite(const char* texture_filename, RenderContext& context);
    Sprite(const char* texture_filename, std::function<void(Sprite*)> deleter, RenderContext& context);
    Sprite(PixelData* texture_data, RenderContext& context);
    Sprite(PixelData* texture_data, std::function<void(Sprite*)> deleter, RenderContext& context);
    ~Sprite();

    void Render(RenderContext& context);
    // This is generally called by object managers to clean up before render api is shut down
    void Finish();

    int index_count();
    TextureResource* texture();
    Vector2 pos();
    Vector2 dimensions();
    MeshData* mesh();

    // The set the position and/or size of sprite
    // as rendered on screen in pixels
    void set_pos(float x, float y);
    void set_pos(float x, float y, float w, float h);
    void set_pos(int x, int y);
    void set_pos(int x, int y, int w, int h);
    // Sets the subregion of sprite texture that is rendered
    // Measured in pixels of texture
    void set_subtexture(float x, float y, float w, float h);
    void set_subtexture(int x, int y, int w, int h);

private:
    void Init(std::function<void(Sprite*)> deleter, RenderContext& context);
    void BuildQuad();

    // Called on destruction
    std::function<void(Sprite*)> deleter_;

    std::unique_ptr<BufferResource> vertex_buffer_, index_buffer_;
    MeshData mesh_;
    std::unique_ptr<Texture> texture_;
    Box pos_;
    Box tex_map_;
};
} // namespace blons
#endif