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

struct ProbeWeight
{
    int probe_id;
    float weight;
};

using CellProbeWeights = std::array<ProbeWeight, 4>;

CellProbeWeights FindProbeWeights(Vector3 world_pos, const LightSector& sector)
{
    CellProbeWeights weights = {};
    const auto& probes = sector.probes();
    const auto& network = sector.probe_network();
    const units::world kMinBarycentricMargin = -0.01f;
    const std::size_t kMaxSteps = network.size();
    int cell_id = 0;
    for (int i = 0; i < kMaxSteps; i++)
    {
        const auto& cell = network[cell_id];

        Vector4 barycentric_pos(world_pos - probes[cell.probe_vertices[0]].pos);
        Vector4 barycentric_coords = barycentric_pos * cell.barycentric_converter;
        barycentric_coords = Vector4(1.0f - barycentric_coords.x - barycentric_coords.y - barycentric_coords.z, barycentric_coords.x, barycentric_coords.y, barycentric_coords.z);
        if (barycentric_coords.x < kMinBarycentricMargin)
        {
            auto next_cell = cell.neighbours[LightSector::FACE_123];
            if (next_cell != -1)
            {
                cell_id = next_cell;
                continue;
            }
            return weights;
        }
        if (barycentric_coords.y < kMinBarycentricMargin)
        {
            auto next_cell = cell.neighbours[LightSector::FACE_023];
            if (next_cell != -1)
            {
                cell_id = next_cell;
                continue;
            }
            return weights;
        }
        if (barycentric_coords.z < kMinBarycentricMargin)
        {
            auto next_cell = cell.neighbours[LightSector::FACE_013];
            if (next_cell != -1)
            {
                cell_id = next_cell;
                continue;
            }
            return weights;
        }
        if (barycentric_coords.w < kMinBarycentricMargin)
        {
            auto next_cell = cell.neighbours[LightSector::FACE_012];
            if (next_cell != -1)
            {
                cell_id = next_cell;
                continue;
            }
            return weights;
        }
        weights[0].probe_id = cell.probe_vertices[0];
        weights[0].weight = std::max(barycentric_coords.x, 0.0f);
        weights[1].probe_id = cell.probe_vertices[1];
        weights[1].weight = std::max(barycentric_coords.y, 0.0f);
        weights[2].probe_id = cell.probe_vertices[2];
        weights[2].weight = std::max(barycentric_coords.z, 0.0f);
        weights[3].probe_id = cell.probe_vertices[3];
        weights[3].weight = std::max(barycentric_coords.w, 0.0f);
        return weights;
    }
    log::Warn("Failed to find probe lighting cell\n");
    return weights;
}
} // namespace

ProbeView::ProbeView()
{
    // Shaders
    ShaderAttributeList probe_inputs;
    probe_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    probe_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    probe_inputs.push_back(ShaderAttribute(ShaderAttributeIndex::NORMAL, "input_norm"));
    probe_inputs.push_back(ShaderAttribute(TANGENT, "input_tan"));
    probe_inputs.push_back(ShaderAttribute(BITANGENT, "input_bitan"));
    probe_shader_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/debug/probe.frag.glsl", probe_inputs));
    ShaderAttributeList grid_shader_inputs;
    grid_shader_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    grid_shader_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/debug/line-grid.frag.glsl", grid_shader_inputs));

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
    auto light_probes = sector.probes();
    Matrix vp_matrix = view_matrix * proj_matrix;
    // Grab camera position from its view matrix. Hacky, lazy, sorry, but not that sorry
    auto inv_view_matrix = MatrixInverse(view_matrix);
    Vector3 camera_pos(inv_view_matrix.m[3][0], inv_view_matrix.m[3][1], inv_view_matrix.m[3][2]);
    auto probe_weights = FindProbeWeights(camera_pos, sector);
    // Bind the buffer to render the probes on top of
    target->Bind(false);
    context->SetDepthTesting(true);
    target->BindDepthTexture(depth);

    // Set the probe-independent inputs
    if (!probe_shader_->SetInput("probe_buffer", sector.probe_shader_data()) ||
        !probe_shader_->SetInput("exposure", scene.view.exposure()) ||
        !probe_shader_->SetInput("debug_mode", debug_mode))
    {
        return false;
    }

    for (const auto& probe : light_probes)
    {
        float weight = 0.0f;
        for (const auto& probe_weight : probe_weights)
        {
            if (probe_weight.probe_id == probe.id)
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

    // Rendering the search network's lines
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
