#ifndef BLONSTECH_GRAPHICS_SPRITE_H_
#define BLONSTECH_GRAPHICS_SPRITE_H_

// Includes
#include <functional>
// Local Includes
#include <blons/graphics/mesh.h>
#include <blons/graphics/texture.h>
#include <blons/graphics/render/render.h>

namespace blons
{
class Sprite
{
public:
    Sprite(std::string texture_filename, RenderContext& context);
    Sprite(PixelData* texture_data, RenderContext& context);
    virtual ~Sprite();

    void Render(RenderContext& context);

    unsigned int vertex_count() const;
    unsigned int index_count() const;
    const TextureResource* texture() const;
    Vector2 pos() const;
    Vector2 dimensions() const;
    const MeshData* mesh();

    // The set the position and/or size of sprite
    // as rendered on screen in pixels
    void set_pos(units::subpixel x, units::subpixel y);
    void set_pos(units::subpixel x, units::subpixel y, units::subpixel w, units::subpixel h);
    void set_pos(units::pixel x, units::pixel y);
    void set_pos(units::pixel x, units::pixel y, units::pixel w, units::pixel h);
    void set_pos(const Box& pos);
    // Sets the subregion of sprite texture that is rendered
    // Measured in pixels of texture
    void set_subtexture(units::subpixel x, units::subpixel y, units::subpixel w, units::subpixel h);
    void set_subtexture(units::pixel x, units::pixel y, units::pixel w, units::pixel h);
    void set_subtexture(const Box& subtexture);

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