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

#include <blons/graphics/pipeline/stage/lightsector/lightsector.h>

// Includes
#include <random>
// Local Includes
#include "radiancetransferbaker.h"

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace
{
auto cvar_gi_boost = console::RegisterVariable("light:gi-boost", 1.0f);
} // namespace

namespace temp
{
void GenerateTestProbe(std::vector<LightSector::Probe>* probes)
{
    LightSector::Probe probe{ static_cast<int>(probes->size()), Vector3(0.0f, 3.0f, 0.0f) };
    probe.brick_factor_range_start = 0;
    probe.brick_factor_count = 0;
    probes->push_back(probe);
}
void GenerateCrytekSponzaProbes(std::vector<LightSector::Probe>* probes)
{
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            for (int z = 0; z < 2; z++)
            {
                LightSector::Probe probe { static_cast<int>(probes->size()), Vector3(-15.0f + x * 1.9f, y * 5.0f + 2.0f, z * 10.0f - 5.0f) };
                probe.brick_factor_range_start = 0;
                probe.brick_factor_count = 0;
                probes->push_back(probe);
            }
        }
    }
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            for (int z = 0; z < 2; z++)
            {
                LightSector::Probe probe { static_cast<int>(probes->size()), Vector3(-15.0f + x * 1.9f, y * 5.0f + 2.0f, z * 2.4f - 1.2f) };
                probe.brick_factor_range_start = 0;
                probe.brick_factor_count = 0;
                probes->push_back(probe);
            }
        }
    }
    for (int x = 0; x < 12; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            for (int z = 0; z < 2; z++)
            {
                LightSector::Probe probe{ static_cast<int>(probes->size()), Vector3(-10.0f + x * 1.727f, y * 2.0f + 10.0f, z * 2.4f - 1.2f) };
                probe.brick_factor_range_start = 0;
                probe.brick_factor_count = 0;
                probes->push_back(probe);
            }
        }
    }
}
void GenerateOldSponzaProbes(std::vector<LightSector::Probe>* probes)
{
    // TODO: Higher light probe density. Will stall load times so should optimize baking in tandem (compute shader? instancing?)
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            for (int z = 0; z < 3; z++)
            {
                LightSector::Probe probe { static_cast<int>(probes->size()), Vector3(-14.0f + x * 4.0f, y * 5.0f + 2.0f, z * 5.0f - 5.0f) };
                probe.brick_factor_range_start = 0;
                probe.brick_factor_count = 0;
                probes->push_back(probe);
            }
        }
    }
    for (int x = 0; x < 6; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            LightSector::Probe probe { static_cast<int>(probes->size()), Vector3(-10.0f + x * 4.0f, y * 3.0f + 11.0f, 0) };
            probe.brick_factor_range_start = 0;
            probe.brick_factor_count = 0;
            probes->push_back(probe);
        }
    }
}
void GenerateRandomProbes(std::vector<LightSector::Probe>* probes, int num_probes, float scale)
{
    std::mt19937 random_algorithm;
    random_algorithm.seed(1);
    std::uniform_real<float> random_distribution(-scale, scale);
    for (int i = 0; i < num_probes; i++)
    {
        LightSector::Probe probe { static_cast<int>(probes->size()), Vector3(random_distribution(random_algorithm),
                                                                             random_distribution(random_algorithm),
                                                                             random_distribution(random_algorithm)) };
        probe.brick_factor_range_start = 0;
        probe.brick_factor_count = 0;
        probes->push_back(probe);
    }
}
} // namespace temp

LightSector::LightSector()
{
    temp::GenerateOldSponzaProbes(&probes_);
    // Initialize shader buffer to fit all probes in
    probe_shader_data_.reset(new ShaderData<LightSector::Probe>(nullptr, probes_.size()));

    // Relight compute shaders to be run every frame
    surfel_brick_relight_shader_.reset(new ComputeShader({ { COMPUTE, "shaders/surfelbrick-relight.comp.glsl" } }));
    probe_relight_shader_.reset(new ComputeShader({ { COMPUTE, "shaders/probe-relight.comp.glsl" } }));
}

void LightSector::BakeRadianceTransfer(const Scene& scene)
{
    // Bake the scene and retrieve the data
    RadianceTransferBaker bake(scene, probes_);
    probes_ = std::vector<Probe>(bake.probes().begin(), bake.probes().end());
    surfels_ = bake.surfels();
    surfel_bricks_ = bake.surfel_bricks();
    surfel_brick_factors_ = bake.surfel_brick_factors();
    probe_network_ = bake.probe_network();
    // Update shader buffer with generated radiance data
    probe_shader_data_.reset(new ShaderData<LightSector::Probe>(probes_.data(), probes_.size()));
    probe_network_shader_data_.reset(new ShaderData<LightSector::ProbeSearchCell>(probe_network_.data(), probe_network_.size()));
    surfel_shader_data_.reset(new ShaderData<LightSector::Surfel>(surfels_.data(), surfels_.size()));
    surfel_brick_shader_data_.reset(new ShaderData<LightSector::SurfelBrick>(surfel_bricks_.data(), surfel_bricks_.size()));
    surfel_brick_factor_shader_data_.reset(new ShaderData<LightSector::SurfelBrickFactor>(surfel_brick_factors_.data(), surfel_brick_factors_.size()));
}

bool LightSector::Relight(const Scene& scene, const Shadow& shadow, Matrix light_vp_matrix)
{
    // Can be removed when we support more lights
    assert(scene.lights.size() == 1);
    Light* sun = scene.lights[0];

    // Iterate over every brick, relighting their surfels and building a radiance term
    if (!surfel_brick_relight_shader_->SetInput("light_vp_matrix", light_vp_matrix) ||
        !surfel_brick_relight_shader_->SetInput("light_depth", shadow.output(Shadow::LIGHT_DEPTH)) ||
        !surfel_brick_relight_shader_->SetInput("sun.dir", sun->direction()) ||
        !surfel_brick_relight_shader_->SetInput("sun.colour", sun->colour()) ||
        !surfel_brick_relight_shader_->SetInput("sun.luminance", sun->luminance()) ||
        !surfel_brick_relight_shader_->SetInput("metalness", Vector3(console::var<float>("mtl:metalness"))) ||
        !surfel_brick_relight_shader_->SetInput("gi_boost", cvar_gi_boost->to<float>()) ||
        !surfel_brick_relight_shader_->SetInput("surfel_buffer", surfel_shader_data()) ||
        !surfel_brick_relight_shader_->SetInput("surfel_brick_buffer", surfel_brick_shader_data()) ||
        !surfel_brick_relight_shader_->SetInput("probe_buffer", probe_shader_data()))
    {
        return false;
    }
    surfel_brick_relight_shader_->Run(static_cast<unsigned int>(surfel_bricks_.size()), 1, 1);

    // Iterate over every probe, building an irradiance term from sky light and any visible surfel bricks
    if (!probe_relight_shader_->SetInput("probe_buffer", probe_shader_data()) ||
        !probe_relight_shader_->SetInput("surfel_brick_buffer", surfel_brick_shader_data()) ||
        !probe_relight_shader_->SetInput("surfel_brick_factor_buffer", surfel_brick_factor_shader_data()) ||
        !probe_relight_shader_->SetInput("sky_luminance", scene.sky_luminance) ||
        !probe_relight_shader_->SetInput("sh_sky_colour.r", scene.sky_box.r.coeffs, 9) ||
        !probe_relight_shader_->SetInput("sh_sky_colour.g", scene.sky_box.g.coeffs, 9) ||
        !probe_relight_shader_->SetInput("sh_sky_colour.b", scene.sky_box.b.coeffs, 9))
    {
        return false;
    }
    probe_relight_shader_->Run(static_cast<unsigned int>(probes_.size()), 1, 1);
    return true;
}

bool LightSector::IsOuterProbeSearchCell(const ProbeSearchCell& cell)
{
    return cell.probe_vertices[3] == INVALID_ID;
}

LightSector::ProbeSearchWeights LightSector::FindProbeWeights(const Vector3& point) const
{
    ProbeSearchWeights weights = {};
    const units::world kMinBarycentricMargin = -0.01f;
    const units::world kMinPlaneDistance = abs(kMinBarycentricMargin) / 2.0f;
    const std::size_t kMaxSteps = probe_network_.size();
    int cell_id = 0;
    for (int i = 0; i < kMaxSteps; i++)
    {
        const auto& cell = probe_network_[cell_id];
        Vector4 barycentric_coords;
        // Barycentric coordinates are calculated by different means for outer and inner cells
        if (IsOuterProbeSearchCell(cell))
        {
            // Collect the positions of the 3 probes in this outer cell
            std::array<Vector3, 3> pos = { probes_[cell.probe_vertices[0]].pos,
                                           probes_[cell.probe_vertices[1]].pos,
                                           probes_[cell.probe_vertices[2]].pos };
            // Collect each vertex normal, pre-scaled during bake to have a distance from the hull plane equal to 1
            const auto& m = cell.barycentric_converter.m;
            std::array<Vector3, 3> normal = { Vector3(m[0][0], m[0][1], m[0][2]),
                                              Vector3(m[1][0], m[1][1], m[1][2]),
                                              Vector3(m[2][0], m[2][1], m[2][2]) };
            Vector3 plane_normal = VectorNormalize(VectorCross(pos[1] - pos[0], pos[2] - pos[0]));
            Vector3 ray = point - pos[0];
            units::world distance_to_plane = VectorDot(plane_normal, ray);
            if (distance_to_plane < kMinPlaneDistance)
            {
                cell_id = cell.neighbours[LightSector::FACE];
                continue;
            }
            // Extrude the hull face to contain our world position
            Triangle extruded_face = { { pos[0] + (normal[0] * distance_to_plane),
                                         pos[1] + (normal[1] * distance_to_plane),
                                         pos[2] + (normal[2] * distance_to_plane) } };
            // Then calculate the barycentric coordinates from the extruded triangle
            barycentric_coords = TriangleBarycentric(extruded_face, point);
            // There are only 3 probes for an outer cell, the last weight will always be 0
            barycentric_coords.w = 0.0f;
            if (barycentric_coords.x < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[LightSector::EDGE_12];
                continue;
            }
            if (barycentric_coords.y < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[LightSector::EDGE_02];
                continue;
            }
            if (barycentric_coords.z < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[LightSector::EDGE_01];
                continue;
            }
        }
        else
        {
            Vector4 barycentric_pos(point - probes_[cell.probe_vertices[0]].pos);
            barycentric_coords = barycentric_pos * cell.barycentric_converter;
            barycentric_coords = Vector4(1.0f - barycentric_coords.x - barycentric_coords.y - barycentric_coords.z, barycentric_coords.x, barycentric_coords.y, barycentric_coords.z);
            if (barycentric_coords.x < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[LightSector::FACE_123];
                continue;
            }
            if (barycentric_coords.y < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[LightSector::FACE_023];
                continue;
            }
            if (barycentric_coords.z < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[LightSector::FACE_013];
                continue;
            }
            if (barycentric_coords.w < kMinBarycentricMargin)
            {
                cell_id = cell.neighbours[LightSector::FACE_012];
                continue;
            }
        }
        weights[0].id = cell.probe_vertices[0];
        weights[0].weight = std::max(barycentric_coords.x, 0.0f);
        weights[1].id = cell.probe_vertices[1];
        weights[1].weight = std::max(barycentric_coords.y, 0.0f);
        weights[2].id = cell.probe_vertices[2];
        weights[2].weight = std::max(barycentric_coords.z, 0.0f);
        weights[3].id = IsOuterProbeSearchCell(cell) ? 0 : cell.probe_vertices[3];
        weights[3].weight = std::max(barycentric_coords.w, 0.0f);
        return weights;
    }
    log::Warn("Failed to find probe lighting cell\n");
    return weights;
}

const std::vector<LightSector::Probe>& LightSector::probes() const
{
    return probes_;
}

const ShaderDataResource* LightSector::probe_shader_data() const
{
    return probe_shader_data_->data();
}

const std::vector<LightSector::ProbeSearchCell>& LightSector::probe_network() const
{
    return probe_network_;
}

const ShaderDataResource* LightSector::probe_network_shader_data() const
{
    return probe_network_shader_data_->data();
}

const std::vector<LightSector::Surfel>& LightSector::surfels() const
{
    return surfels_;
}

const ShaderDataResource* LightSector::surfel_shader_data() const
{
    // TODO: Remove this when light sector streaming is implemented and we can get PRT data in constructor
    if (surfel_shader_data_ == nullptr)
    {
        throw "Attempted to access surfel shader data before light bake";
    }
    return surfel_shader_data_->data();
}

const std::vector<LightSector::SurfelBrick>& LightSector::surfel_bricks() const
{
    return surfel_bricks_;
}

const ShaderDataResource* LightSector::surfel_brick_shader_data() const
{
    // TODO: Remove this when light sector streaming is implemented and we can get PRT data in constructor
    if (surfel_brick_shader_data_ == nullptr)
    {
        throw "Attempted to access surfel brick shader data before light bake";
    }
    return surfel_brick_shader_data_->data();
}

const std::vector<LightSector::SurfelBrickFactor>& LightSector::surfel_brick_factors() const
{
    return surfel_brick_factors_;
}

const ShaderDataResource* LightSector::surfel_brick_factor_shader_data() const
{
    // TODO: Remove this when light sector streaming is implemented and we can get PRT data in constructor
    if (surfel_brick_factor_shader_data_ == nullptr)
    {
        throw "Attempted to access surfel brick shader data before light bake";
    }
    return surfel_brick_factor_shader_data_->data();
}
} // namespace stage
} // namespace pipeline
} // namespace blons
