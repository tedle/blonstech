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

#include <blons/graphics/pipeline/stage/debug/surfelview.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace debug
{
namespace
{
auto const cvar_debug_mode = console::RegisterVariable("dbg:surfel-view", 0);
} // namespace

SurfelView::SurfelView()
    : quad_mesh_("blons:quad")
{
    // Shaders
    ShaderAttributeList surfel_shader_inputs = { { POS, "input_pos" } };
    surfel_shader_.reset(new Shader({ { VERTEX, "shaders/debug/surfel.vert.glsl" }, { PIXEL, "shaders/debug/surfel.frag.glsl" } }, surfel_shader_inputs));

    if (surfel_shader_ == nullptr)
    {
        throw "Failed to initialize debug Surfel shaders";
    }
}

bool SurfelView::Render(Framebuffer* target, const TextureResource* depth, const Scene& scene, const LightSector& sector, Matrix view_matrix, Matrix proj_matrix)
{
    auto debug_mode = cvar_debug_mode->to<int>();
    if (!debug_mode)
    {
        return true;
    }

    auto context = render::context();
    context->SetDepthTesting(true);
    context->SetBlendMode(BlendMode::ALPHA);
    // Bind the buffer to render the surfels on top of
    target->Bind(false);
    context->SetDepthTesting(true);
    target->BindDepthTexture(depth);
    context->BeginScene(Vector4());

    // Our quad mesh vertices range from -1,1 so we divide by 2 to represent surfel size
    Matrix world_matrix = MatrixScale(kSurfelSize / 2.0f, kSurfelSize / 2.0f, kSurfelSize / 2.0f);
    Matrix vp_matrix = view_matrix * proj_matrix;

    // Bind the quad mesh for instanced rendering
    render::context()->BindMeshBuffer(quad_mesh_.buffer());
    // Set the inputs
    if (!surfel_shader_->SetInput("world_matrix", world_matrix) ||
        !surfel_shader_->SetInput("vp_matrix", vp_matrix) ||
        !surfel_shader_->SetInput("exposure", scene.view.exposure()) ||
        !surfel_shader_->SetInput("surfel_buffer", sector.surfel_shader_data()))
    {
        target->BindDepthTexture(target->depth());
        return false;
    }
    // Run the shader
    if (!surfel_shader_->RenderInstanced(quad_mesh_.index_count(), static_cast<unsigned int>(sector.surfels().size())))
    {
        target->BindDepthTexture(target->depth());
        return false;
    }

    target->BindDepthTexture(target->depth());
    return true;
}
} // namespace debug
} // namespace stage
} // namespace pipeline
} // namespace blons
