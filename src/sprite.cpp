#include "graphics/sprite.h"

namespace blons
{
Sprite::Sprite(const char* texture_filename, RenderContext& context)
{
    texture_ = std::unique_ptr<Texture>(new Texture(texture_filename, Texture::Type::SPRITE, context));
    Init(context);
}

Sprite::Sprite(PixelData* texture_data, RenderContext& context)
{
    texture_ = std::unique_ptr<Texture>(new Texture(texture_data, Texture::Type::SPRITE, context));
    Init(context);
}

void Sprite::Init(RenderContext& context)
{
    vertex_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    index_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    Texture::Info dimensions = texture_->info();
    set_pos(0, 0, dimensions.width, dimensions.height);
    set_subtexture(0, 0, dimensions.width, dimensions.height);

    mesh_.indices.resize(6);
    mesh_.indices = { 0, 1, 2, 1, 3, 2 };

    mesh_.vertices.resize(4);
    BuildQuad();

    if (!context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(),
                                 mesh_.vertices.data(), mesh_.vertices.size(),
                                 mesh_.indices.data(), mesh_.indices.size()))
    {
        throw "Failed to register sprite";
    }
}

Sprite::~Sprite()
{
}

void Sprite::Render(RenderContext& context)
{
    BuildQuad();
    context->SetMeshData(vertex_buffer_.get(), index_buffer_.get(),
                         mesh_.vertices.data(), mesh_.vertices.size(),
                         mesh_.indices.data(), mesh_.indices.size());
    context->BindMeshBuffer(vertex_buffer_.get(), index_buffer_.get());
}

int Sprite::index_count()
{
    // 6, lol
    return mesh_.indices.size();
}

TextureResource* Sprite::texture()
{
    return texture_->texture();
}

Vector2 Sprite::pos()
{
    return Vector2(pos_.x, pos_.y);
}

Vector2 Sprite::dimensions()
{
    return Vector2(pos_.w, pos_.h);
}

void Sprite::set_pos(float x, float y)
{
    set_pos(x, y, pos_.w, pos_.h);
}

void Sprite::set_pos(float x, float y, float w, float h)
{
    pos_.x = x;
    pos_.y = y;
    pos_.w = w;
    pos_.h = h;
}

void Sprite::set_pos(int x, int y)
{
    set_pos(static_cast<float>(x), static_cast<float>(y));
}

void Sprite::set_pos(int x, int y, int w, int h)
{
    set_pos(static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(w),
            static_cast<float>(h));
}

void Sprite::set_subtexture(float x, float y, float w, float h)
{
    tex_map_.x = x;
    tex_map_.y = y;
    tex_map_.w = w;
    tex_map_.h = h;
}

void Sprite::set_subtexture(int x, int y, int w, int h)
{
    set_subtexture(static_cast<float>(x),
                   static_cast<float>(y),
                   static_cast<float>(w),
                   static_cast<float>(h));
}

MeshData* Sprite::mesh()
{
    BuildQuad();
    return &mesh_;
}

void Sprite::BuildQuad()
{
    // Faster than working with the vector object
    auto* vs = mesh_.vertices.data();

    vs[0].pos.x = pos_.x;          vs[0].pos.y = pos_.y;
    vs[1].pos.x = pos_.x;          vs[1].pos.y = pos_.y + pos_.h;
    vs[2].pos.x = pos_.x + pos_.w; vs[2].pos.y = pos_.y;
    vs[3].pos.x = pos_.x + pos_.w; vs[3].pos.y = pos_.y + pos_.h;

    Texture::Info info = texture_->info();
    Box normal_tex(tex_map_.x / info.width,
                   tex_map_.y / info.height,
                   tex_map_.w / info.width,
                   tex_map_.h / info.height);

    vs[0].tex.x = normal_tex.x;                vs[0].tex.y = normal_tex.y + normal_tex.h;
    vs[1].tex.x = normal_tex.x;                vs[1].tex.y = normal_tex.y;
    vs[2].tex.x = normal_tex.x + normal_tex.w; vs[2].tex.y = normal_tex.y + normal_tex.h;
    vs[3].tex.x = normal_tex.x + normal_tex.w; vs[3].tex.y = normal_tex.y;
}
} // namespace blons