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

#include <blons/graphics/pipeline/stage/geometry.h>

// Public Includes
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
Geometry::Geometry(Perspective perspective, RenderContext& context)
{
    // Shaders
    ShaderAttributeList geometry_inputs;
    geometry_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    geometry_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    geometry_inputs.push_back(ShaderAttribute(ShaderAttributeIndex::NORMAL, "input_norm"));
    geometry_inputs.push_back(ShaderAttribute(TANGENT, "input_tan"));
    geometry_inputs.push_back(ShaderAttribute(BITANGENT, "input_bitan"));
    geometry_shader_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/mesh.frag.glsl", geometry_inputs, context));

    if (geometry_shader_ == nullptr)
    {
        throw "Failed to initialize Geometry shaders";
    }

    // Framebuffers
    geometry_buffer_.reset(new Framebuffer(perspective.width, perspective.height, 4, context));
}

bool Geometry::Render(const Scene& scene, Matrix view_matrix, Matrix proj_matrix, RenderContext& context)
{
    // Needed so models dont render over themselves
    context->SetDepthTesting(true);

    // Bind the geometry framebuffer to render all models onto
    geometry_buffer_->Bind(context);

    Matrix view_proj = view_matrix * proj_matrix;
    // TODO: 3D pass ->
    //      Render static world geo as batches without world matrix
    //      Render movable objects singularly with world matrix
    for (const auto& model : scene.models)
    {
        // Bind the vertex data
        model->Render(context);

        Matrix model_view_proj = model->world_matrix() * view_proj;
        // Set the inputs
        if (!geometry_shader_->SetInput("mvp_matrix", model_view_proj, context) ||
            !geometry_shader_->SetInput("albedo", model->albedo(), 0, context) ||
            !geometry_shader_->SetInput("normal", model->normal(), 1, context))
        {
            return false;
        }

        // Make the draw call
        if (!geometry_shader_->Render(model->index_count(), context))
        {
            return false;
        }
    }
    return true;
}

const TextureResource* Geometry::output(Output buffer) const
{
    switch (buffer)
    {
    case ALBEDO:
        return geometry_buffer_->textures()[0];
        break;
    case NORMAL:
        return geometry_buffer_->textures()[1];
        break;
    case DEPTH:
        return geometry_buffer_->depth();
        break;
    case DEBUG:
        return geometry_buffer_->textures()[2];
        break;
    default:
        return nullptr;
    }
}
} // namespace stage
} // namespace pipeline
} // namespace blons