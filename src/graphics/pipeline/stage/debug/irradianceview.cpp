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
#include <algorithm>
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

IrradianceView::IrradianceView()
{
    // Shaders
    ShaderAttributeList grid_shader_inputs;
    grid_shader_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    grid_shader_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    grid_shader_inputs.push_back(ShaderAttribute(ShaderAttributeIndex::NORMAL, "input_norm"));
    grid_shader_inputs.push_back(ShaderAttribute(TANGENT, "input_tan"));
    grid_shader_inputs.push_back(ShaderAttribute(BITANGENT, "input_bitan"));
    grid_shader_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/debug-irradiance-grid.frag.glsl", grid_shader_inputs));
    ShaderAttributeList volume_shader_inputs;
    volume_shader_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    volume_shader_inputs.push_back(ShaderAttribute(ShaderAttributeIndex::NORMAL, "input_norm"));
    volume_shader_inputs.push_back(ShaderAttribute(TANGENT, "input_grid_pos"));
    volume_shader_.reset(new Shader("shaders/debug-irradiance-volume.vert.glsl", "shaders/debug-irradiance-volume.frag.glsl", volume_shader_inputs));

    if (volume_shader_ == nullptr || grid_shader_ == nullptr)
    {
        throw "Failed to initialize debug Irradiance Volume shaders";
    }
}

bool IrradianceView::Render(Framebuffer* target, const TextureResource* depth, const Scene& scene, const IrradianceVolume& irradiance, Matrix view_matrix, Matrix proj_matrix)
{
    auto debug_mode = cvar_debug_mode->to<int>();
    if (!debug_mode)
    {
        return true;
    }

    // Mesh initialization is deferred because it's optional and adds significant startup time
    if (grid_mesh_ == nullptr || voxel_meshes_ == nullptr)
    {
        InitMeshBuffers(irradiance);
    }

    auto context = render::context();
    // Bind the buffer to render the volume on top of
    target->Bind(false);
    context->SetDepthTesting(true);
    target->BindDepthTexture(depth);

    Matrix vp_matrix = view_matrix * proj_matrix;

    // Rendering the volume's grid lines
    grid_mesh_->Render(false);
    // Set the inputs
    if (!grid_shader_->SetInput("mvp_matrix", irradiance.world_matrix() * vp_matrix))
    {
        target->BindDepthTexture(target->depth());
        return false;
    }
    // Run the shader
    if (!grid_shader_->Render(grid_mesh_->index_count()))
    {
        target->BindDepthTexture(target->depth());
        return false;
    }

    // Rendering the volume's voxels
    voxel_meshes_->Render(false);
    // Set the inputs
    if (!volume_shader_->SetInput("world_matrix", irradiance.world_matrix()) ||
        !volume_shader_->SetInput("vp_matrix", vp_matrix) ||
        !volume_shader_->SetInput("irradiance_volume_px", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_PX), 0) ||
        !volume_shader_->SetInput("irradiance_volume_nx", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_NX), 1) ||
        !volume_shader_->SetInput("irradiance_volume_py", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_PY), 2) ||
        !volume_shader_->SetInput("irradiance_volume_ny", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_NY), 3) ||
        !volume_shader_->SetInput("irradiance_volume_pz", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_PZ), 4) ||
        !volume_shader_->SetInput("irradiance_volume_nz", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_NZ), 5) ||
        !volume_shader_->SetInput("exposure", scene.view.exposure()))
    {
        target->BindDepthTexture(target->depth());
        return false;
    }
    // Run the shader
    if (!volume_shader_->Render(voxel_meshes_->index_count()))
    {
        target->BindDepthTexture(target->depth());
        return false;
    }
    target->BindDepthTexture(target->depth());
    return true;
}

void IrradianceView::InitMeshBuffers(const IrradianceVolume& irradiance)
{
    // Grab the dimensions of the irradiance volume and construct a matching grid mesh
    auto grid_data = render::context()->GetTextureData3D(irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_PX));
    std::stringstream dimension_args;
    dimension_args << (grid_data.width) << "," << (grid_data.height) << "," << (grid_data.depth);
    grid_mesh_.reset(new DrawBatcher(DrawMode::LINES));
    Mesh grid_mesh("blons:line-grid~" + dimension_args.str());
    grid_mesh_->Append(grid_mesh.mesh());

    // Construct a cloud of cubes positioned to each voxel of the irradiance volume
    voxel_meshes_.reset(new DrawBatcher());
    Mesh cube_mesh("blons:cube");
    MeshData cube_mesh_data = cube_mesh.mesh();
    Matrix cube_scale = MatrixScale(0.05f, 0.05f, 0.05f);
    MeshData cloud_mesh_data;
    cloud_mesh_data.draw_mode = DrawMode::TRIANGLES;
    // Pre-allocate for optimization (it helps! really!)
    cloud_mesh_data.vertices.reserve(grid_data.width * grid_data.height * grid_data.depth * cube_mesh_data.vertices.size());
    cloud_mesh_data.indices.reserve(grid_data.width * grid_data.height * grid_data.depth * cube_mesh_data.indices.size());
    Vector3 grid_dimensions(static_cast<units::world>(grid_data.width),
                            static_cast<units::world>(grid_data.height),
                            static_cast<units::world>(grid_data.depth));
    for (int x = 0; x < grid_data.width; x++)
    {
        for (int y = 0; y < grid_data.height; y++)
        {
            for (int z = 0; z < grid_data.depth; z++)
            {
                for (auto& v : cube_mesh_data.vertices)
                {
                    // We use the tangent memory to store each cube's texel space position
                    v.tan = Vector3(static_cast<units::world>(x),
                                    static_cast<units::world>(y),
                                    static_cast<units::world>(z));
                    // Offset by half a texel so we sample the center
                    v.tan += Vector3(0.5f);
                    v.tan /= grid_dimensions;
                }
                // Insert modified cube into cube cloud
                cloud_mesh_data.vertices.insert(cloud_mesh_data.vertices.end(), cube_mesh_data.vertices.begin(), cube_mesh_data.vertices.end());
                cloud_mesh_data.indices.insert(cloud_mesh_data.indices.end(), cube_mesh_data.indices.begin(), cube_mesh_data.indices.end());
                // Add current vertex count to newly inserted indices
                std::transform(cube_mesh_data.indices.begin(), cube_mesh_data.indices.end(),
                               cloud_mesh_data.indices.end() - cube_mesh_data.indices.size(),
                               [&](auto i) { return i + static_cast<unsigned int>(cloud_mesh_data.vertices.size() - cube_mesh_data.vertices.size()); });
            }
        }
    }
    voxel_meshes_->Append(cloud_mesh_data, cube_scale);
}
} // namespace debug
} // namespace stage
} // namespace pipeline
} // namespace blons
