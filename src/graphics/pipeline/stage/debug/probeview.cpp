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

#include <blons/graphics/pipeline/stage/debug/probeview.h>

// Public Includes
#include <blons/graphics/pipeline/stage/lightprobes.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/render/shader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace debug
{
ProbeView::ProbeView()
{
    // Shaders
    ShaderAttributeList probe_inputs;
    probe_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    probe_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    probe_inputs.push_back(ShaderAttribute(ShaderAttributeIndex::NORMAL, "input_norm"));
    probe_inputs.push_back(ShaderAttribute(TANGENT, "input_tan"));
    probe_inputs.push_back(ShaderAttribute(BITANGENT, "input_bitan"));
    probe_shader_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/debug-shprobe.frag.glsl", probe_inputs));

    if (probe_shader_ == nullptr)
    {
        throw "Failed to initialize debug SH Probe shaders";
    }
    probe_meshes_.reset(new DrawBatcher());
    std::unique_ptr<Mesh> probe_mesh(new Mesh("blons:sphere~0.5"));
    MeshData probe_mesh_data = probe_mesh->mesh();
    probe_meshes_->Append(probe_mesh_data);
}

bool ProbeView::Render(Framebuffer* target, const TextureResource* depth, const LightProbes& probes, Matrix view_matrix, Matrix proj_matrix, Matrix ortho_matrix)
{
    auto context = render::context();
    // Bind the buffer to render the probes on top of
    target->Bind(false);
    context->SetDepthTesting(true);

    Matrix vp_matrix = view_matrix * proj_matrix;
    Matrix cube_face_projection = MatrixPerspective(kPi / 2.0f, 1.0f, 0.1f, 10000.0f);

    for (const auto& probe : probes.probes_)
    {
        // Bind the vertex data
        probe_meshes_->Render(false);

        // Set the inputs
        if (!probe_shader_->SetInput("mvp_matrix", MatrixTranslation(probe.pos.x, probe.pos.y, probe.pos.z) * vp_matrix) ||
            !probe_shader_->SetInput("env_proj_matrix", cube_face_projection) ||
            !probe_shader_->SetInput("env_tex_size", kProbeMapSize) ||
            !probe_shader_->SetInput("probe_count", static_cast<int>(probes.probes_.size())) ||
            !probe_shader_->SetInput("probe_id", static_cast<int>(probe.id)) ||
            !probe_shader_->SetInput("probe_env_maps", probes.environment_maps_->textures()[0], 0))
        {
            return false;
        }

        // Finally do the render
        target->BindDepthTexture(depth);
        if (!probe_shader_->Render(probe_meshes_->index_count()))
        {
            target->BindDepthTexture(target->depth());
            return false;
        }
    }
    target->BindDepthTexture(target->depth());
    return true;
}
} // namespace debug
} // namespace stage
} // namespace pipeline
} // namespace blons
