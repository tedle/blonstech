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

#include <blons/graphics/framebuffer.h>

namespace blons
{
Framebuffer::Framebuffer(units::pixel width, units::pixel height, unsigned int texture_outputs, RenderContext& context)
{
    fbo_.reset(context->MakeFramebufferResource());
    if (!context->RegisterFramebuffer(fbo_.get(), width, height, texture_outputs))
    {
        fbo_.reset();
        throw "Failed to initialize framebuffer";
    }

    // Keep a copy of the texture resources so we don't need the context to fetch them
    for (const auto& t : context->FramebufferTextures(fbo_.get()))
    {
        textures_.push_back(t);
    }
    depth_texture_ = context->FramebufferDepthTexture(fbo_.get());

    vertex_buffer_.reset(context->MakeBufferResource());
    index_buffer_.reset(context->MakeBufferResource());

    // Build a simple quad that can render the outputs to screen
    render_quad_.vertices.resize(4);
    render_quad_.indices.resize(6);
    auto* vs = render_quad_.vertices.data();

    vs[0].pos.x = 0.0f;                            vs[0].pos.y = 0.0f;
    vs[1].pos.x = 0.0f;                            vs[1].pos.y = units::pixel_to_subpixel(height);
    vs[2].pos.x = units::pixel_to_subpixel(width); vs[2].pos.y = units::pixel_to_subpixel(height);
    vs[3].pos.x = units::pixel_to_subpixel(width); vs[3].pos.y = 0.0f;

    // Invert texture coordinates cus opengl saves them bottom-left
    // TODO: Find a render API independant solution for this
    vs[0].tex.x = 0; vs[0].tex.y =  0;
    vs[1].tex.x = 0; vs[1].tex.y = -1;
    vs[2].tex.x = 1; vs[2].tex.y = -1;
    vs[3].tex.x = 1; vs[3].tex.y =  0;

    render_quad_.indices = { 0, 1, 2, 0, 2, 3 };

    if (!context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(),
        render_quad_.vertices.data(), vertex_count(),
        render_quad_.indices.data(), index_count()))
    {
        throw "Failed to register rendering quad";
    }
}

void Framebuffer::Render(RenderContext& context)
{
    context->BindMeshBuffer(vertex_buffer_.get(), index_buffer_.get());
}

void Framebuffer::Bind(RenderContext& context)
{
    context->BindFramebuffer(fbo_.get());
    context->BeginScene();
}

void Framebuffer::Unbind(RenderContext& context)
{
    context->BindFramebuffer(nullptr);
}

unsigned int Framebuffer::vertex_count() const
{
    // 4
    std::size_t vertex_count = render_quad_.vertices.size();
    return static_cast<unsigned int>(vertex_count);
}

unsigned int Framebuffer::index_count() const
{
    // 6
    std::size_t index_count = render_quad_.indices.size();
    return static_cast<unsigned int>(index_count);
}

std::vector<const TextureResource*> Framebuffer::textures()
{
    return textures_;
}

const TextureResource* Framebuffer::depth()
{
    return depth_texture_;
}
} // namespace blons