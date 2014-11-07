#include "sprite.h"

namespace blons
{
Sprite::Sprite(const char* texture_filename, RenderContext& context)
{
    vertex_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    index_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    texture_ = std::unique_ptr<Texture>(new Texture(texture_filename, Texture::Type::DIFFUSE, context));
    pos_.x = 0;
    pos_.y = 0;
    width_.x = 100;
    width_.y = 500;

    indices_.resize(6);
    indices_ = { 0, 1, 2, 1, 3, 2 };
    Vertex vert;
    vert.pos.x = pos_.x;   vert.pos.y = pos_.y;   vertices_.push_back(vert);
    vert.pos.x = pos_.x;   vert.pos.y = width_.y; vertices_.push_back(vert);
    vert.pos.x = width_.x; vert.pos.y = pos_.y;   vertices_.push_back(vert);
    vert.pos.x = width_.x; vert.pos.y = width_.y; vertices_.push_back(vert);

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
    context->SetModelBuffer(vertex_buffer_.get(), index_buffer_.get());
}

TextureResource* Sprite::texture()
{
    return texture_->texture();
}
} // namespace blons