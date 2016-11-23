////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/graphics/sprite.h>

namespace blons
{
Sprite::Sprite(std::string texture_filename, TextureType::Options options)
{
    texture_.reset(new Texture(texture_filename, options));
    Init();
}

Sprite::Sprite(std::string texture_filename)
    : Sprite::Sprite(texture_filename, { TextureType::RAW, TextureType::NEAREST, TextureType::REPEAT })
{
}

Sprite::Sprite(const PixelData& texture_data)
{
    texture_.reset(new Texture(texture_data));
    Init();
}

void Sprite::Init()
{
    auto context = render::context();
    vertex_buffer_.reset(context->MakeBufferResource());
    index_buffer_.reset(context->MakeBufferResource());
    Texture::Info dimensions = texture_->info();
    set_pos(0, 0, dimensions.width, dimensions.height);
    set_subtexture(0, 0, dimensions.width, dimensions.height);

    mesh_.indices.resize(6);
    // Order to render the quad's vertices in
    mesh_.indices = { 0, 1, 2, 0, 2, 3 };

    mesh_.vertices.resize(4);
    BuildQuad();

    if (!context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(),
                                 mesh_.vertices.data(), vertex_count(),
                                 mesh_.indices.data(), index_count()))
    {
        throw "Failed to register sprite";
    }
}

void Sprite::Render()
{
    BuildQuad();
    auto context = render::context();
    context->UpdateMeshData(vertex_buffer_.get(), index_buffer_.get(),
                            mesh_.vertices.data(), 0, vertex_count(),
                            mesh_.indices.data(), 0, index_count());
    context->BindMeshBuffer(vertex_buffer_.get(), index_buffer_.get());
}

void Sprite::Reload()
{
    // Backup our sprite settings
    auto temp_pos = pos_;
    auto temp_tex = tex_map_;

    // Freshly initialize
    Init();
    texture_->Reload();

    // Restore & push settings
    pos_ = temp_pos;
    tex_map_ = temp_tex;
    BuildQuad();
    render::context()->UpdateMeshData(vertex_buffer_.get(), index_buffer_.get(),
                                      mesh_.vertices.data(), 0, vertex_count(),
                                      mesh_.indices.data(), 0, index_count());
}

unsigned int Sprite::vertex_count() const
{
    // 4
    std::size_t vertex_count = mesh_.vertices.size();
    return static_cast<unsigned int>(vertex_count);
}

unsigned int Sprite::index_count() const
{
    // 6, lol
    std::size_t index_count = mesh_.indices.size();
    return static_cast<unsigned int>(index_count);
}

const PixelData* Sprite::pixels(bool force_gpu_sync)
{
    return texture_->pixels(force_gpu_sync);
}

const TextureResource* Sprite::texture() const
{
    return texture_->texture();
}

Vector2 Sprite::pos() const
{
    return Vector2(pos_.x, pos_.y);
}

Vector2 Sprite::dimensions() const
{
    return Vector2(pos_.w, pos_.h);
}

void Sprite::set_pos(units::subpixel x, units::subpixel y)
{
    set_pos(x, y, pos_.w, pos_.h);
}

void Sprite::set_pos(units::subpixel x, units::subpixel y, units::subpixel w, units::subpixel h)
{
    pos_.x = x;
    pos_.y = y;
    pos_.w = w;
    pos_.h = h;
}

void Sprite::set_pos(units::pixel x, units::pixel y)
{
    set_pos(units::pixel_to_subpixel(x), units::pixel_to_subpixel(y));
}

void Sprite::set_pos(units::pixel x, units::pixel y, units::pixel w, units::pixel h)
{
    set_pos(units::pixel_to_subpixel(x),
            units::pixel_to_subpixel(y),
            units::pixel_to_subpixel(w),
            units::pixel_to_subpixel(h));
}

void Sprite::set_pos(const Box& pos)
{
    set_pos(pos.x, pos.y, pos.w, pos.h);
}

void Sprite::set_subtexture(units::subpixel x, units::subpixel y, units::subpixel w, units::subpixel h)
{
    tex_map_.x = x;
    tex_map_.y = y;
    tex_map_.w = w;
    tex_map_.h = h;
}

void Sprite::set_subtexture(units::pixel x, units::pixel y, units::pixel w, units::pixel h)
{
    set_subtexture(units::pixel_to_subpixel(x),
                   units::pixel_to_subpixel(y),
                   units::pixel_to_subpixel(w),
                   units::pixel_to_subpixel(h));
}

void Sprite::set_subtexture(const Box& subtexture)
{
    set_subtexture(subtexture.x, subtexture.y, subtexture.w, subtexture.h);
}

const MeshData& Sprite::mesh()
{
    BuildQuad();
    return mesh_;
}

void Sprite::BuildQuad()
{
    // Faster than working with the vector object
    auto* vs = mesh_.vertices.data();

    vs[0].pos.x = pos_.x;          vs[0].pos.y = pos_.y;
    vs[1].pos.x = pos_.x;          vs[1].pos.y = pos_.y + pos_.h;
    vs[2].pos.x = pos_.x + pos_.w; vs[2].pos.y = pos_.y + pos_.h;
    vs[3].pos.x = pos_.x + pos_.w; vs[3].pos.y = pos_.y;

    // Texture region is stored in pixels, but graphics API needs it normalized as [0,1]
    Texture::Info info = texture_->info();
    Box normal_tex(tex_map_.x / info.width,
                   tex_map_.y / info.height,
                   tex_map_.w / info.width,
                   tex_map_.h / info.height);

    vs[0].tex.x = normal_tex.x;                vs[0].tex.y = normal_tex.y;
    vs[1].tex.x = normal_tex.x;                vs[1].tex.y = normal_tex.y + normal_tex.h;
    vs[2].tex.x = normal_tex.x + normal_tex.w; vs[2].tex.y = normal_tex.y + normal_tex.h;
    vs[3].tex.x = normal_tex.x + normal_tex.w; vs[3].tex.y = normal_tex.y;
}
} // namespace blons