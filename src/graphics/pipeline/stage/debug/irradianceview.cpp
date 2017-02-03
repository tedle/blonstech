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

#include <blons/graphics/pipeline/stage/debug/irradianceview.h>

// Includes
#include <sstream>

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
auto const cvar_debug_mode = console::RegisterVariable("dbg:irradiance-view", 0);
} // namespace

IrradianceView::IrradianceView(const IrradianceVolume& irradiance)
{
    // Shaders
    ShaderAttributeList shader_inputs;
    shader_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    shader_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    shader_inputs.push_back(ShaderAttribute(ShaderAttributeIndex::NORMAL, "input_norm"));
    shader_inputs.push_back(ShaderAttribute(TANGENT, "input_tan"));
    shader_inputs.push_back(ShaderAttribute(BITANGENT, "input_bitan"));
    volume_shader_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/debug-irradiance-volume.frag.glsl", shader_inputs));
    grid_shader_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/debug-irradiance-grid.frag.glsl", shader_inputs));

    if (volume_shader_ == nullptr || grid_shader_ == nullptr)
    {
        throw "Failed to initialize debug Irradiance Volume shaders";
    }

    // Grab the dimensions of the irradiance volume and construct a matching grid mesh
    auto grid_data = render::context()->GetTextureData3D(irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME));
    std::stringstream dimension_args;
    dimension_args << grid_data.width << "," << grid_data.height << "," << grid_data.depth;
    grid_mesh_.reset(new DrawBatcher(DrawMode::LINES));
    std::unique_ptr<Mesh> grid_mesh(new Mesh("blons:line-grid~" + dimension_args.str()));
    grid_mesh_->Append(grid_mesh->mesh());
}

bool IrradianceView::Render(Framebuffer* target, const TextureResource* depth, const IrradianceVolume& irradiance, Matrix view_matrix, Matrix proj_matrix)
{
    auto debug_mode = cvar_debug_mode->to<int>();
    if (!debug_mode)
    {
        return true;
    }

    auto context = render::context();
    // Bind the buffer to render the probes on top of
    target->Bind(false);
    context->SetDepthTesting(true);
    target->BindDepthTexture(depth);

    Matrix vp_matrix = view_matrix * proj_matrix;

    // Bind the vertex data
    grid_mesh_->Render(false);

    // Set the inputs
    if (!grid_shader_->SetInput("mvp_matrix", irradiance.world_matrix() * vp_matrix))
    {
        target->BindDepthTexture(target->depth());
        return false;
    }
    if (!grid_shader_->Render(grid_mesh_->index_count()))
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
