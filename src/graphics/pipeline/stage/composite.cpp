////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
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

#include <blons/graphics/pipeline/stage/composite.h>

// Public Includes
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
Composite::Composite(Perspective perspective)
{
    // Shaders
    ShaderAttributeList composite_inputs;
    composite_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    composite_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    composite_shader_.reset(new Shader({ { VERTEX, "shaders/sprite.vert.glsl" }, { PIXEL, "shaders/composite.frag.glsl" } }, composite_inputs));

    if (composite_shader_ == nullptr)
    {
        throw "Failed to initialize compositing shaders";
    }

    // Framebuffers
    composite_buffer_.reset(new Framebuffer(perspective.width, perspective.height, 1, false));
}

bool Composite::Render(const TextureResource* scene, const TextureResource* debug_output, Matrix ortho_matrix)
{
    auto context = render::context();
    context->SetDepthTesting(false);
    context->SetBlendMode(BlendMode::OVERWRITE);

    // Bind the buffer to do all lighting calculations on
    composite_buffer_->Bind();

    composite_buffer_->Render();

    // Set the inputs
    if (!composite_shader_->SetInput("proj_matrix", ortho_matrix) ||
        !composite_shader_->SetInput("scene", scene, 0) ||
        !composite_shader_->SetInput("debug_output", debug_output, 1))
    {
        return false;
    }

    // Finally do the render
    if (!composite_shader_->Render(composite_buffer_->index_count()))
    {
        return false;
    }

    return true;
}

const TextureResource* Composite::output(Output buffer) const
{
    switch (buffer)
    {
    case FINAL:
        return composite_buffer_->textures()[0];
        break;
    default:
        return nullptr;
    }
}
} // namespace stage
} // namespace pipeline
} // namespace blons