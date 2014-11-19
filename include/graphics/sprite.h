#ifndef BLONSTECH_GRAPHICS_SPRITE_H_
#define BLONSTECH_GRAPHICS_SPRITE_H_

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
    Sprite(const char* texture_filename, RenderContext& context);
    Sprite(PixelData* texture_data, RenderContext& context);
    virtual ~Sprite();

    void Render(RenderContext& context);

    int index_count() const;
    const TextureResource* texture() const;
    Vector2 pos() const;
    Vector2 dimensions() const;
    const MeshData* mesh();

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

protected:
    void Init(RenderContext& context);
    void BuildQuad();

    std::unique_ptr<BufferResource> vertex_buffer_, index_buffer_;
    MeshData mesh_;
    std::unique_ptr<Texture> texture_;
    Box pos_;
    Box tex_map_;
};
} // namespace blons
#endif