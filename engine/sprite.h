#ifndef BLONSTECH_SPRITE_H_
#define BLONSTECH_SPRITE_H_

// Local Includes
#include "render.h"
#include "texture.h"

namespace blons
{
class Sprite
{
public:
    Sprite(const char* texture_filename, RenderContext& context);
    ~Sprite();

    void Render(RenderContext& context);

    TextureResource* texture();
    Vector2 pos();

    void set_pos(int x, int y);

private:
    std::unique_ptr<BufferResource> vertex_buffer_, index_buffer_;
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::unique_ptr<Texture> texture_;
    Vector2 pos_;
    Vector2 width_;
};
} // namespace blons
#endif