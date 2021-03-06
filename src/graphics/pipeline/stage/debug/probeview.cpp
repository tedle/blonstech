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

#include <blons/graphics/pipeline/stage/debug/probeview.h>

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
auto const cvar_debug_mode = console::RegisterVariable("dbg:probe-view", 0);
} // namespace

ProbeView::ProbeView()
{
    // Shaders
    ShaderAttributeList probe_inputs = { { POS, "input_pos" },
                                         { TEX, "input_uv" },
                                         { ShaderAttributeIndex::NORMAL, "input_norm" },
                                         { TANGENT, "input_tan" },
                                         { BITANGENT, "input_bitan" } };
    probe_shader_.reset(new Shader({ { VERTEX, "shaders/mesh.vert.glsl" }, { PIXEL, "shaders/debug/probe.frag.glsl" } }, probe_inputs));
    ShaderAttributeList grid_shader_inputs = { { POS, "input_pos" } };
    grid_shader_.reset(new Shader({ { VERTEX, "shaders/mesh.vert.glsl" }, { PIXEL, "shaders/debug/line-grid.frag.glsl" } }, grid_shader_inputs));

    if (probe_shader_ == nullptr || grid_shader_ == nullptr)
    {
        throw "Failed to initialize debug Probe shaders";
    }
}

bool ProbeView::Render(Framebuffer* target, const TextureResource* depth, const Scene& scene, const LightSector& sector, Matrix view_matrix, Matrix proj_matrix)
{
    auto debug_mode = cvar_debug_mode->to<int>();
    if (!debug_mode)
    {
        return true;
    }

    // Mesh initialization is deferred because probe network is not available during startup
    if (probe_meshes_ == nullptr || probe_network_mesh_ == nullptr)
    {
        InitMeshBuffers(sector);
    }

    auto context = render::context();
    context->SetDepthTesting(true);
    context->SetBlendMode(BlendMode::ALPHA);

    auto light_probes = sector.probes();
    Matrix vp_matrix = view_matrix * proj_matrix;
    // Grab camera position from its view matrix. Hacky, lazy, sorry, but not that sorry
    auto inv_view_matrix = MatrixInverse(view_matrix);
    Vector3 camera_pos(inv_view_matrix.m[3][0], inv_view_matrix.m[3][1], inv_view_matrix.m[3][2]);
    auto probe_weights = sector.FindProbeWeights(camera_pos);
    // Bind the buffer to render the probes on top of
    target->Bind(false);
    target->BindDepthTexture(depth);

    // Set the probe-independent inputs
    if (!probe_shader_->SetInput("probe_buffer", sector.probe_shader_data()) ||
        !probe_shader_->SetInput("probe_network_buffer", sector.probe_network_shader_data()) ||
        !probe_shader_->SetInput("exposure", scene.view.exposure()) ||
        !probe_shader_->SetInput("camera_position", scene.view.pos()) ||
        !probe_shader_->SetInput("debug_mode", debug_mode))
    {
        return false;
    }

    for (const auto& probe : light_probes)
    {
        float weight = 0.0f;
        for (const auto& probe_weight : probe_weights)
        {
            if (probe_weight.id == probe.id)
            {
                weight = probe_weight.weight;
                break;
            }
        }
        // Bind the vertex data
        probe_meshes_->Render(false);

        Matrix world_matrix = MatrixTranslation(probe.pos.x, probe.pos.y, probe.pos.z);
        // Set the probe-specific inputs
        if (!probe_shader_->SetInput("mvp_matrix",  world_matrix * vp_matrix) ||
            !probe_shader_->SetInput("normal_matrix", MatrixTranspose(MatrixInverse(world_matrix))) ||
            !probe_shader_->SetInput("probe_id", static_cast<int>(probe.id)) ||
            !probe_shader_->SetInput("probe_weight", weight))
        {
            return false;
        }

        // Finally do the render
        if (!probe_shader_->Render(probe_meshes_->index_count()))
        {
            target->BindDepthTexture(target->depth());
            return false;
        }
    }

    // Rendering the search network's lines if relevant to the debug mode
    if (debug_mode == 4)
    {
        probe_network_mesh_->Render(false);
        // Set the inputs
        if (!grid_shader_->SetInput("mvp_matrix", view_matrix * proj_matrix))
        {
            target->BindDepthTexture(target->depth());
            return false;
        }
        // Run the shader
        if (!grid_shader_->Render(probe_network_mesh_->index_count()))
        {
            target->BindDepthTexture(target->depth());
            return false;
        }
    }

    target->BindDepthTexture(target->depth());
    return true;
}

void ProbeView::InitMeshBuffers(const LightSector& sector)
{
    // Sphere used for rendering probes
    probe_meshes_.reset(new DrawBatcher());
    std::unique_ptr<Mesh> probe_mesh(new Mesh("blons:sphere"));
    MeshData probe_mesh_data = probe_mesh->mesh();
    probe_meshes_->Append(probe_mesh_data, MatrixScale(0.5f, 0.5f, 0.5f));

    // Lines used for rendering probe search network
    probe_network_mesh_.reset(new DrawBatcher(DrawMode::LINES));
    auto network = sector.probe_network();
    auto probes = sector.probes();
    MeshData lines;
    lines.draw_mode = DrawMode::LINES;
    // So we don't waste time re-allocating while building this mesh
    lines.vertices.reserve(network.size() * 4);
    lines.indices.reserve(network.size() * 12);
    for (const auto& cell : network)
    {
        // We don't visualize outer cells
        if (sector.IsOuterProbeSearchCell(cell))
        {
            continue;
        }
        auto offset = static_cast<unsigned int>(lines.vertices.size());
        // Copy over the 4 vertices needed to draw this tetrahedron
        lines.vertices.push_back({ probes[cell.probe_vertices[0]].pos });
        lines.vertices.push_back({ probes[cell.probe_vertices[1]].pos });
        lines.vertices.push_back({ probes[cell.probe_vertices[2]].pos });
        lines.vertices.push_back({ probes[cell.probe_vertices[3]].pos });
        // Generate the indices needed to draw the 6 edges
        std::vector<unsigned int> indices = { 0, 1, 0, 2, 0, 3, 1, 2, 1, 3, 2, 3 };
        std::transform(indices.begin(), indices.end(), std::back_inserter(lines.indices), [&](const auto& index) { return index + offset; });
    }
    probe_network_mesh_->Append(lines);
}
} // namespace debug
} // namespace stage
} // namespace pipeline
} // namespace blons
