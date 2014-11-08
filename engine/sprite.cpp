#include "sprite.h"

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
    pos_ = Box(0, 0, dimensions.width, dimensions.height);
    tex_map_ = Box(0, 0, 1, 1);

    indices_.resize(6);
    indices_ = { 0, 1, 2, 1, 3, 2 };

    vertices_.resize(4);
    BuildQuad();

    if (!context->RegisterQuad(vertex_buffer_.get(), index_buffer_.get(),
                               vertices_.data(), vertices_.size(),
                               indices_.data(), indices_.size()))
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
    context->SetQuadData(vertex_buffer_.get(), vertices_.data(), vertices_.size());
    context->BindModelBuffer(vertex_buffer_.get(), index_buffer_.get());
}

int Sprite::index_count()
{
    // 6, lol
    return indices_.size();
}

TextureResource* Sprite::texture()
{
    return texture_->texture();
}

Vector2 Sprite::pos()
{
    return Vector2(pos_.x, pos_.y);
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

void Sprite::BuildQuad()
{
    vertices_[0].pos.x = pos_.x;          vertices_[0].pos.y = pos_.y;
    vertices_[1].pos.x = pos_.x;          vertices_[1].pos.y = pos_.y + pos_.h;
    vertices_[2].pos.x = pos_.x + pos_.w; vertices_[2].pos.y = pos_.y;
    vertices_[3].pos.x = pos_.x + pos_.w; vertices_[3].pos.y = pos_.y + pos_.h;

    vertices_[0].tex.x = tex_map_.x;              vertices_[0].tex.y = tex_map_.y + tex_map_.h;
    vertices_[1].tex.x = tex_map_.x;              vertices_[1].tex.y = tex_map_.y;
    vertices_[2].tex.x = tex_map_.x + tex_map_.w; vertices_[2].tex.y = tex_map_.y + tex_map_.h;
    vertices_[3].tex.x = tex_map_.x + tex_map_.w; vertices_[3].tex.y = tex_map_.y;
}
} // namespace blons