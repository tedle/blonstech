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

#include "radiancetransferbaker.h"

// Includes
#include <algorithm>
#include <array>
#include <numeric>

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace
{
const std::vector<AxisAlignedNormal> kFaceOrder = { NEGATIVE_Z, POSITIVE_X, POSITIVE_Z, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y };
const int kProbeNetworkFaces = 4;
const int kProbeNetworkEdges = 3;
// When weighting the function f(theta,phi) = 1 over
// a sphere with UV-spaced inputs this will result
// in a final sum of 4*pi*N where N is the number of
// texels sampled
// uv should be in normalized device coordinates [-1,1]
float EnvironmentMapTexelWeight(const Vector2& uv)
{
    // 1 = uv.z^2
    float texel_weight_intermediate = uv.x * uv.x + uv.y * uv.y + 1.0f;
    // 4 = Volume of Monte Carlo integration dx=[-1,1],dy=[-1,1]
    // *
    // 6 = Number of faces to integrate
    return 24.0f / (sqrt(texel_weight_intermediate) * texel_weight_intermediate);
}
} // namespace

RadianceTransferBaker::RadianceTransferBaker(const Scene& scene, const std::vector<LightSector::Probe>& probes)
    : probes_(probes)
{
    // Setup shader for generating environment maps to build surfel and sky visibility data
    ShaderAttributeList env_map_inputs = { { POS, "input_pos" },
                                           { TEX, "input_uv" },
                                           { ShaderAttributeIndex::NORMAL, "input_norm" },
                                           { TANGENT, "input_tan" },
                                           { BITANGENT, "input_bitan" } };
    environment_map_shader_.reset(new Shader({ { VERTEX, "shaders/probe-env-map.vert.glsl" }, { PIXEL, "shaders/probe-env-map.frag.glsl" } }, env_map_inputs));
    // Setup framebuffer for storing environment maps
    environment_maps_.reset(new Framebuffer(kProbeMapSize * 6, kProbeMapSize * static_cast<units::pixel>(probes_.size()),
                                            { { TextureType::R8G8B8A8, TextureType::RAW, TextureType::NEAREST, TextureType::CLAMP },   // albedo + sky vis
                                              { TextureType::R8G8B8,   TextureType::RAW, TextureType::NEAREST, TextureType::CLAMP } }, // normal
                                            true));

    // G-Buffer env map generation
    log::Debug("Baking environment maps... ");
    Timer env_bake_stats;
    BakeEnvironmentMaps(scene);
    log::Debug("[%ims]\n", env_bake_stats.ms());
    // Gather surfel and sky visibility samples from probes
    log::Debug("Gathering probe samples... ");
    Timer sample_gather_stats;
    std::vector<SurfelSample> surfel_samples;
    std::vector<SkyVisSample> sky_samples;
    GatherProbeSamples(&surfel_samples, &sky_samples);
    log::Debug("[%ims]\n", sample_gather_stats.ms());
    // Compute surfel clusters
    log::Debug("Baking surfel clusters... ");
    Timer surfel_bake_stats;
    BakeSurfelClusters(surfel_samples);
    log::Debug("[%ims]\n", surfel_bake_stats.ms());
    // Compute SH coefficients for sky visibility
    log::Debug("Baking sky visibility... ");
    Timer sky_bake_stats;
    BakeSkyCoefficients(sky_samples);
    log::Debug("[%ims]\n", sky_bake_stats.ms());
    // Create a Delaunay Triangulation for probe interpolation
    log::Debug("Baking probe network...");
    Timer bake_network_stats;
    BakeProbeNetwork();
    log::Debug("[%ims]\n", bake_network_stats.ms());
}

const std::vector<LightSector::Probe>& RadianceTransferBaker::probes() const
{
    return probes_;
}

const std::vector<LightSector::ProbeSearchCell>& RadianceTransferBaker::probe_network() const
{
    return probe_network_;
}

const std::vector<LightSector::Surfel>& RadianceTransferBaker::surfels() const
{
    return surfels_;
}

const std::vector<LightSector::SurfelBrick>& RadianceTransferBaker::surfel_bricks() const
{
    return surfel_bricks_;
}

const std::vector<LightSector::SurfelBrickFactor>& RadianceTransferBaker::surfel_brick_factors() const
{
    return surfel_brick_factors_;
}

void RadianceTransferBaker::BakeEnvironmentMaps(const Scene& scene)
{
    // Shader data delivery struct
    struct PerFaceData
    {
        Matrix vp_matrix;
        int scissor_x;
        int scissor_y;
    };
    std::vector<PerFaceData> per_face_data;

    auto context = render::context();
    environment_maps_->Bind(Vector4(0, 1, 0, 1));
    context->SetDepthTesting(true);
    context->SetBlendMode(BlendMode::OVERWRITE);

    Camera cube_view;
    const Matrix cube_projection = MatrixPerspective(kPi / 2.0f, 1.0f, kBakeScreenNear, kBakeScreenFar, render::context()->IsDepthBufferRangeZeroToOne());

    // Build up a buffer of unique face data used for instanced rendering
    for (const auto& probe : probes_)
    {
        cube_view.set_pos(probe.pos.x, probe.pos.y, probe.pos.z);
        int face_index = 0;
        for (const auto& face : kFaceOrder)
        {
            PerFaceData face_data;
            Vector3 rot = AxisRotationPitchYawRoll(face);
            cube_view.set_rot(rot.x, rot.y, rot.z);
            face_data.vp_matrix = cube_view.view_matrix() * cube_projection;
            face_data.scissor_x = face_index * kProbeMapSize;
            face_data.scissor_y = static_cast<units::pixel>(probe.id) * kProbeMapSize;
            per_face_data.push_back(face_data);
            face_index++;
        }
    }
    // Setup any non-varying shader inputs
    ShaderData<PerFaceData> per_face_shaderdata(per_face_data.data(), per_face_data.size());
    environment_map_shader_->SetInput("per_face_data_buffer", per_face_shaderdata.data());
    environment_map_shader_->SetInput("scissor_w", kProbeMapSize);
    environment_map_shader_->SetInput("scissor_h", kProbeMapSize);
    environment_map_shader_->SetInput("map_width", kProbeMapSize * 6);
    environment_map_shader_->SetInput("map_height", kProbeMapSize * static_cast<units::pixel>(probes_.size()));
    // Render each model a total of (number of probes) * (6 faces) times
    for (const auto& m : scene.models)
    {
        m->Render();
        environment_map_shader_->SetInput("m_matrix", m->world_matrix());
        environment_map_shader_->SetInput("normal_matrix", MatrixTranspose(MatrixInverse(m->world_matrix())));
        environment_map_shader_->SetInput("albedo", m->albedo(), 0);
        environment_map_shader_->SetInput("normal", m->normal(), 1);
        environment_map_shader_->RenderInstanced(m->index_count(), static_cast<unsigned int>(per_face_data.size()));
    }

    environment_maps_->Unbind();
}

void RadianceTransferBaker::GatherProbeSamples(std::vector<SurfelSample>* surfel_samples, std::vector<SkyVisSample>* sky_samples)
{
    // Pre-allocate sample containers
    surfel_samples->reserve(kProbeMapSize * kProbeMapSize * 6 * probes_.size());
    sky_samples->reserve(kProbeMapSize * kProbeMapSize * 6 * probes_.size());

    // Retrieve textures and pixel spacing in memory
    auto albedo_tex = render::context()->GetTextureData(environment_maps_->textures()[0], 0);
    auto normal_tex = render::context()->GetTextureData(environment_maps_->textures()[1], 0);
    auto depth_tex = render::context()->GetTextureData(environment_maps_->depth(), 0);
    std::size_t albedo_pixel_size = albedo_tex.bits_per_pixel() / 8;
    std::size_t normal_pixel_size = normal_tex.bits_per_pixel() / 8;
    std::size_t depth_pixel_size = depth_tex.bits_per_pixel() / 8;
    const Matrix cube_projection = MatrixPerspective(kPi / 2.0f, 1.0f, kBakeScreenNear, kBakeScreenFar, render::context()->IsDepthBufferRangeZeroToOne());

    // Iterate over each face of each probe and generate samples
    for (const auto& probe : probes_)
    {
        int face_index = 0;
        for (const auto& face : kFaceOrder)
        {
            // Reconstruct camera rotation that was used to render scene for this face
            Vector3 rot = AxisRotationPitchYawRoll(face);
            Camera cube_view;
            cube_view.set_pos(0, 0, 0);
            cube_view.set_rot(rot.x, rot.y, rot.z);
            // Since a view matrix rotates things in the opposite of the direction given
            // We use the inverse for determining the normal of the sphere at a given texel
            Matrix sphere_rotation_matrix = MatrixInverse(cube_view.view_matrix());
            // Reconstruct full camera rotation and position that was used to render scene for this face
            // Used for reconstructing the world space position at a given texel, same as in deferred rendering
            cube_view.set_pos(probe.pos.x, probe.pos.y, probe.pos.z);
            Matrix inverse_vp_matrix = MatrixInverse(cube_view.view_matrix() * cube_projection);

            // Finally for each texel: generate and store a sample
            for (int x = 0; x < kProbeMapSize; x++)
            {
                for (int y = 0; y < kProbeMapSize; y++)
                {
                    // Normalized Device Coordinates of texel
                    Vector2 uv;
                    uv.x = (static_cast<units::world>(x) + 0.5f) / static_cast<units::world>(kProbeMapSize) * 2.0f - 1.0f;
                    uv.y = (static_cast<units::world>(y) + 0.5f) / static_cast<units::world>(kProbeMapSize) * 2.0f - 1.0f;
                    // Texel coordinates
                    int px = x + face_index * kProbeMapSize;
                    int py = y + probe.id * kProbeMapSize;

                    // Extract and translate sample data from environment maps
                    auto albedo = Vector3(static_cast<float>(albedo_tex.pixels.data()[(px + py * albedo_tex.width) * albedo_pixel_size + 0]) / 255.0f,
                                          static_cast<float>(albedo_tex.pixels.data()[(px + py * albedo_tex.width) * albedo_pixel_size + 1]) / 255.0f,
                                          static_cast<float>(albedo_tex.pixels.data()[(px + py * albedo_tex.width) * albedo_pixel_size + 2]) / 255.0f);
                    // Albedo alpha channel holds sky visibility
                    auto sky_visibility = static_cast<float>(albedo_tex.pixels.data()[(px + py * albedo_tex.width) * albedo_pixel_size + 3]) / 255.0f;
                    // Translate from texture encoded normal to world space normal
                    auto surface_normal = Vector3(static_cast<float>(normal_tex.pixels.data()[(px + py * normal_tex.width) * normal_pixel_size + 0]) / 255.0f,
                                                  static_cast<float>(normal_tex.pixels.data()[(px + py * normal_tex.width) * normal_pixel_size + 1]) / 255.0f,
                                                  static_cast<float>(normal_tex.pixels.data()[(px + py * normal_tex.width) * normal_pixel_size + 2]) / 255.0f);
                    surface_normal = VectorNormalize(surface_normal * 2.0f - 1.0f);
                    // Depth value is stored as a float across 4 unsigned chars so we cast to a pointer and then dereference
                    auto depth = *reinterpret_cast<units::world*>(&depth_tex.pixels.data()[(px + py * depth_tex.width) * depth_pixel_size]);
                    // Translate to normalized device coordinates
                    depth = depth * 2.0f - 1.0f;

                    // View direction from probe to sample
                    Vector3 sphere_normal(uv.x, uv.y, -1.0f);
                    sphere_normal *= sphere_rotation_matrix;
                    sphere_normal = VectorNormalize(sphere_normal);

                    // Finally build and store each sample
                    if (sky_visibility < 0.5f)
                    {
                        // World space position of given sample
                        Vector4 world_pos(uv.x, uv.y, depth, 1.0f);
                        world_pos *= inverse_vp_matrix;
                        world_pos /= world_pos.w;

                        SurfelSample surfel_sample;
                        LightSector::Surfel surfel;
                        surfel.nearest_probe_id = probe.id;
                        surfel.pos = Vector3(world_pos.x, world_pos.y, world_pos.z);
                        surfel.normal = surface_normal;
                        surfel.albedo = albedo;
                        surfel_sample.surfel = surfel;
                        surfel_sample.parent_probe = probe.id;
                        // Generate 6 weights for this sample based on the cosine to each axis
                        Vector3 sample_dir = VectorNormalize(surfel.pos - probe.pos);
                        for (const auto& weight_face : kFaceOrder)
                        {
                            Vector3 axis_dir;
                            switch (weight_face)
                            {
                            case POSITIVE_X:
                                axis_dir = Vector3(1.0, 0.0, 0.0);
                                break;
                            case NEGATIVE_X:
                                axis_dir = Vector3(-1.0, 0.0, 0.0);
                                break;
                            case POSITIVE_Y:
                                axis_dir = Vector3(0.0, 1.0, 0.0);
                                break;
                            case NEGATIVE_Y:
                                axis_dir = Vector3(0.0, -1.0, 0.0);
                                break;
                            case POSITIVE_Z:
                                axis_dir = Vector3(0.0, 0.0, 1.0);
                                break;
                            case NEGATIVE_Z:
                                axis_dir = Vector3(0.0, 0.0, -1.0);
                                break;
                            }
                            surfel_sample.parent_probe_weights[weight_face] = std::max(VectorDot(axis_dir, sample_dir), 0.0f);
                            // Weight by texel solid angle since we are approximating a hemispherical function
                            surfel_sample.parent_probe_weights[weight_face] *= EnvironmentMapTexelWeight(uv);
                        }
                        surfel_samples->push_back(surfel_sample);
                    }
                    SkyVisSample sky_sample;
                    sky_sample.uv = uv;
                    sky_sample.normal = sphere_normal;
                    sky_sample.visibility = sky_visibility;
                    sky_sample.parent_probe = probe.id;
                    sky_samples->push_back(sky_sample);
                }
            }
            face_index++;
        }
    }
}

void RadianceTransferBaker::BakeSurfelClusters(const std::vector<SurfelSample>& samples)
{
    // Turn surfel samples into spatially indexed clusters
    auto surfel_data = ClusterSurfelData(samples);
    auto brick_data = ClusterBrickData(&surfel_data);
    // Transfer clustered surfel and brick data to contiguous memory
    // BakeBrick data still needed for weight factor generation
    std::vector<BakeBrick> contiguous_bricks;
    contiguous_bricks.reserve(brick_data.size());
    for (auto& brick : brick_data)
    {
        // Calculate the final index references
        brick.second.brick.surfel_range_start = static_cast<int>(surfels_.size());
        brick.second.brick.surfel_count = static_cast<int>(brick.second.surfels.size());
        // Transfer to contiguous memory
        contiguous_bricks.push_back(brick.second);
        surfel_bricks_.push_back(brick.second.brick);
        // Cluster final surfels by brick
        for (const auto& bake_surfel : brick.second.surfels)
        {
            surfels_.push_back(bake_surfel.surfel);
        }
    }
    // Turn use BakeBrick data to build brick weights for each probe
    GenerateBrickWeights(contiguous_bricks);
    // Normalize weights to PI
    NormalizeBrickWeights();
}

RadianceTransferBaker::SurfelCluster RadianceTransferBaker::ClusterSurfelData(const std::vector<SurfelSample>& samples)
{
    // Build a spatial, clustered hash grid of all surfels
    SurfelCluster surfel_data;
    // The max possible number of surfels we'll store (likely much less)
    surfel_data.reserve(samples.size());
    // Spatially index each sample, summing surfel data in overlapping indices
    // These are averaged later in the bake process during ClusterBrickData (sloppier code, but more efficient sorry)
    for (const auto& sample : samples)
    {
        SurfelIndex search_index;
        // Offset negative values by a whole step to deal with naive truncations resulting
        // in too many surfels located at index 0
        search_index.x = static_cast<int>(sample.surfel.pos.x / kSurfelSize) + (sample.surfel.pos.x >= 0.0 ? 0 : -1);
        search_index.y = static_cast<int>(sample.surfel.pos.y / kSurfelSize) + (sample.surfel.pos.y >= 0.0 ? 0 : -1);
        search_index.z = static_cast<int>(sample.surfel.pos.z / kSurfelSize) + (sample.surfel.pos.z >= 0.0 ? 0 : -1);
        search_index.direction = FindGreatestAxis(sample.surfel.normal);
        // Copy parent probe id and basis weights
        BakeSurfel::ParentProbeData parent_data;
        parent_data.id = sample.parent_probe;
        std::copy(std::begin(sample.parent_probe_weights), std::end(sample.parent_probe_weights), std::begin(parent_data.weights));
        // Check if surfel exists already
        auto it = surfel_data.find(search_index);
        // Add to surfel at index
        if (it != surfel_data.end())
        {
            it->second.surfel.pos += sample.surfel.pos;
            it->second.surfel.normal += sample.surfel.normal;
            it->second.surfel.albedo += sample.surfel.albedo;
            it->second.parent_probes.push_back(parent_data);
            it->second.sample_count++;
        }
        // Generate new surfel at index
        else
        {
            BakeSurfel surfel;
            surfel.surfel = sample.surfel;
            surfel.parent_probes = { parent_data };
            surfel.sample_count = 1;
            surfel_data[search_index] = surfel;
        }
    }
    return surfel_data;
}

RadianceTransferBaker::BrickCluster RadianceTransferBaker::ClusterBrickData(SurfelCluster* surfel_data)
{
    // Build a spatial, clustered hash grid of all bricks
    BrickCluster brick_data;
    // The max possible number of bricks we'll store (likely much less)
    brick_data.reserve(surfel_data->size());
    // Cluster surfels into bricks
    for (auto& surfel_it : *surfel_data)
    {
        auto& s = surfel_it.second;
        // Average surfel values among given samples
        s.surfel.pos    /= static_cast<units::world>(s.sample_count);
        s.surfel.normal /= static_cast<units::world>(s.sample_count);
        s.surfel.normal = VectorNormalize(s.surfel.normal);
        s.surfel.albedo /= static_cast<units::world>(s.sample_count);
        s.sample_count = 1;
        // Also an efficient time to solve for nearest probe
        for (const auto& parent_probe : s.parent_probes)
        {
            if (s.surfel.nearest_probe_id != parent_probe.id)
            {
                auto dist_old = VectorLength(probes_[s.surfel.nearest_probe_id].pos - s.surfel.pos);
                auto dist_new = VectorLength(probes_[parent_probe.id].pos - s.surfel.pos);
                if (dist_new < dist_old)
                {
                    s.surfel.nearest_probe_id = parent_probe.id;
                }
            }
        }

        SurfelIndex search_index;
        // Offset negative values by a whole step and 1 to deal with naive truncations resulting
        // in too many bricks located at index 0
        search_index.x = (surfel_it.first.x + (surfel_it.first.x >= 0 ? 0 : -kSurfelsPerBrick + 1)) / kSurfelsPerBrick;
        search_index.y = (surfel_it.first.y + (surfel_it.first.y >= 0 ? 0 : -kSurfelsPerBrick + 1)) / kSurfelsPerBrick;
        search_index.z = (surfel_it.first.z + (surfel_it.first.z >= 0 ? 0 : -kSurfelsPerBrick + 1)) / kSurfelsPerBrick;
        search_index.direction = surfel_it.first.direction;

        auto brick_it = brick_data.find(search_index);
        if (brick_it != brick_data.end())
        {
            brick_it->second.surfels.push_back(s);
        }
        else
        {
            BakeBrick brick;
            brick.surfels = { s };
            brick_data[search_index] = brick;
        }
    }
    return brick_data;
}

void RadianceTransferBaker::GenerateBrickWeights(const std::vector<BakeBrick>& bricks)
{
    // Generate brick factors
    std::vector<BakeBrickFactor> brick_factor_data;
    for (int i = 0; i < bricks.size(); i++)
    {
        // Stores number of samples each probe face saw in this brick
        std::unordered_map<int, float[6]> probe_counts;
        for (const auto& surfel : bricks[i].surfels)
        {
            // Iterate through every sample that was gathered from this surfel
            // Note the sampling probe face that sampled and add to its weight
            for (const auto& probe : surfel.parent_probes)
            {
                auto probe_it = probe_counts.find(probe.id);
                // Add to sample count
                if (probe_it != probe_counts.end())
                {
                    for (const auto& face : kFaceOrder)
                    {
                        probe_it->second[face] += probe.weights[face];
                    }
                }
                // Generate new sample count array
                else
                {
                    for (const auto& face : kFaceOrder)
                    {
                        probe_counts[probe.id][face] = probe.weights[face];
                    }
                }
            }
        }
        // Build a BakeBrickFactor for each probe that samples this brick using
        // the weights we just generated
        for (const auto& probe : probe_counts)
        {
            BakeBrickFactor f;
            f.factor.brick_id = i;
            // Copy basis weights
            std::copy(std::begin(probe.second), std::end(probe.second), std::begin(f.factor.brick_weights));
            f.parent_probe = probe.first;
            brick_factor_data.push_back(f);
        }
    }
    // Group by parent probe
    std::sort(brick_factor_data.begin(), brick_factor_data.end(),
              [](const auto& a, const auto& b){ return a.parent_probe < b.parent_probe; });
    // Fill in brick factor indices for parent probes
    // Transfer bake brick factors to final container in the same pass
    for (int i = 0; i < brick_factor_data.size(); i++)
    {
        const auto& factor = brick_factor_data[i];
        auto& probe = probes_[factor.parent_probe];
        if (probe.brick_factor_count == 0)
        {
            probe.brick_factor_range_start = i;
        }
        probe.brick_factor_count++;
        // Transfer to contiguous memory
        surfel_brick_factors_.push_back(factor.factor);
    }
}

void RadianceTransferBaker::NormalizeBrickWeights()
{
    // Applying this will make brick weights for a given basis sum roughly to pi.
    // It's not exact (within 0.3% at low resolution), but building an exact
    // normalization factor would ignore the dip in contribution that should
    // occur when many samples are sky facing. Larger probe map resolutions
    // approach closer to pi
    // This is dervied thru use of the EnvironmentMapTexelWeight function which
    // is designed to make all samples sum to 4*pi*N for the function f(theta,phi) = 1.
    // We are aiming for a sum of pi for the function f(theta,phi) = max(cos(phi),0).
    // The integral of these 2 functions differs by a factor of 4 which cancels out
    // the 4 of the weight function for free. This means the normalization factor for
    // both use cases is simply N, or 6*kProbeMapSize^2
    const float normalization_factor = static_cast<float>(kProbeMapSize * kProbeMapSize * 6);
    // Normalize brick weights to pi
    for (const auto& probe : probes_)
    {
        auto start = surfel_brick_factors_.begin() + probe.brick_factor_range_start;
        auto end = start + probe.brick_factor_count;
        // Apply normalization factor
        std::transform(start, end, start,
        [&](auto bf)
        {
            for (int face = 0; face < 6; face++)
            {
                bf.brick_weights[face] /= normalization_factor;
            }
            return bf;
        });
    }
}

void RadianceTransferBaker::BakeSkyCoefficients(const std::vector<SkyVisSample>& samples)
{
    // Holds normalization factor for each probe as samples are summed
    std::vector<float> probe_weights(probes_.size(), 0.0f);
    // Iterate over all samples and sum up sky coefficients
    for (const auto& sample : samples)
    {
        // Weight texels contribution by its solid angle on the sphere
        float sh_texel_weight = EnvironmentMapTexelWeight(sample.uv);
        // Add sample to SH coefficients
        probes_[sample.parent_probe].sh_sky_visibility += SHProjectDirection3(sample.normal) * sample.visibility * sh_texel_weight;
        // Build up normalization sums
        probe_weights[sample.parent_probe] += sh_texel_weight;
    }
    // Normalize sky coefficients to surface area of unit sphere
    for (auto& probe : probes_)
    {
        probe.sh_sky_visibility *= 4.0f * kPi / probe_weights[probe.id];
    }
}

void RadianceTransferBaker::BakeProbeNetwork()
{
    // Probe lookup acceleration and interpolation structure based on:
    // http://www.gdcvault.com/play/1015312/Light-Probe-Interpolation-Using-Tetrahedral
    auto triangulation = TriangulateProbeNetwork();
    BakeProbeNetworkInnerCells(triangulation);
    BakeProbeNetworkInnerNeighbours();
    BakeProbeNetworkOuterCells();
    BakeProbeNetworkOuterNeighbours();
    BakeProbeNetworkHullNormals();
    BakeProbeNetworkConvererters();
}

std::vector<Tetrahedron> RadianceTransferBaker::TriangulateProbeNetwork()
{
    // Bowyer-Watson algorithm for calculating Delaunay triangulations
    std::vector<Tetrahedron> tetrahedrons;
    // Create 5 bounding tetrahedrons that encase all probe positions
    // Start by making an AABB
    Vector3 min(std::numeric_limits<units::world>::max());
    Vector3 max(-std::numeric_limits<units::world>::max());
    for (const auto& probe : probes_)
    {
        min.x = std::min(min.x, probe.pos.x);
        max.x = std::max(max.x, probe.pos.x);
        min.y = std::min(min.y, probe.pos.y);
        max.y = std::max(max.y, probe.pos.y);
        min.z = std::min(min.z, probe.pos.z);
        max.z = std::max(max.z, probe.pos.z);
    }
    // Add a margin because of floating point precision
    min -= 5.0f;
    max += 5.0f;
    // Build a bounding box volume out of 5 tetrahedrons
    const std::vector<Tetrahedron> kBoundingTetrahedrons = {
        { Vector3(max.x, min.y, min.z), Vector3(min.x, max.y, min.z), Vector3(min.x, min.y, max.z), min },
        { Vector3(min.x, max.y, min.z), Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, min.z), max },
        { Vector3(min.x, max.y, min.z), Vector3(min.x, min.y, max.z), Vector3(min.x, max.y, max.z), max },
        { Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, min.z), Vector3(max.x, min.y, max.z), max },
        { Vector3(min.x, max.y, min.z), Vector3(max.x, min.y, min.z), Vector3(min.x, min.y, max.z), max }
    };
    // Inject them into our solver as a starting point
    tetrahedrons.insert(tetrahedrons.begin(), kBoundingTetrahedrons.begin(), kBoundingTetrahedrons.end());
    for (const auto& probe : probes_)
    {
        Vector3 point = probe.pos;
        // Partition the tetrahedrons so that circumspheres containing point are in the latter half (returns false)
        auto partition_point = std::partition(tetrahedrons.begin(), tetrahedrons.end(), [&](const auto& tetrahedron)
        {
            Sphere circumsphere = TetrahedronCircumsphere(tetrahedron);
            // 1e-3 is a magic number. I hope you never have to deal with it.
            // This implementation gets messed up on heavily co-planar point sets.
            // I have spent a lot of time messing with this and I'm not sure why. I even had
            // an infinite precision float library hooked up at one point to make sure it wasn't
            // caused by rounding errors (didn't seem to be). I found 1e-3 just by exasperatedly
            // tweaking numbers. Making this number smaller or larger seems to ruin its mysterious
            // properties. There is a throwable exception lying in wait should this ever run into
            // a dataset that causes it to fail.
            return !(VectorDistance(circumsphere.center, point) <= circumsphere.radius - 1e-3);
        });
        // Move the marked tetrahedrons to a new list
        std::vector<Tetrahedron> subdividable_tetrahedrons(std::make_move_iterator(partition_point),
                                                           std::make_move_iterator(tetrahedrons.end()));
        // And remove them from the old one
        tetrahedrons.erase(partition_point, tetrahedrons.end());

        // Define a triangle type as 3 points
        using Triangle = std::array<Vector3, 3>;

        // Build a list of every face within the list of subdividable tetrahedrons
        std::vector<Triangle> triangle_list;
        for (const auto& tetrahedron : subdividable_tetrahedrons)
        {
            triangle_list.push_back({ tetrahedron.vertices[0], tetrahedron.vertices[1], tetrahedron.vertices[2] }); // Skip 3
            triangle_list.push_back({ tetrahedron.vertices[0], tetrahedron.vertices[1], tetrahedron.vertices[3] }); // Skip 2
            triangle_list.push_back({ tetrahedron.vertices[0], tetrahedron.vertices[2], tetrahedron.vertices[3] }); // Skip 1
            triangle_list.push_back({ tetrahedron.vertices[1], tetrahedron.vertices[2], tetrahedron.vertices[3] }); // Skip 0
        }
        // Sort every triangle so they can be compared
        for (auto& triangle : triangle_list)
        {
            std::stable_sort(triangle.begin(), triangle.end(), [](const auto& a, const auto& b) { return a.x < b.x; });
            std::stable_sort(triangle.begin(), triangle.end(), [](const auto& a, const auto& b) { return a.y < b.y; });
            std::stable_sort(triangle.begin(), triangle.end(), [](const auto& a, const auto& b) { return a.z < b.z; });
        }
        // Remove both instances (yes, both) of every duplicate triangle face
        std::vector<Triangle> unique_triangle_list;
        std::copy_if(triangle_list.begin(), triangle_list.end(), std::back_inserter(unique_triangle_list), [&](const auto& triangle)
        {
            int match_count = 0;
            for (const auto& other_triangle : triangle_list)
            {
                // If I read the standard right this should be fine since we never did any floating point math with these.
                // While risky, doing this properly is preferrable over an epsilon since that's a whole can of worms that
                // could quietly bite you down the road
                if (triangle[0] == other_triangle[0] && triangle[1] == other_triangle[1] && triangle[2] == other_triangle[2])
                {
                    match_count++;
                }
            }
            // This can only be caused by degenerate tetrahedrons
            if (match_count != 2 && match_count != 1)
            {
                // If we're getting this again and have the time to do a full overhaul of the triangulation
                // here's a method that I think will work better with floating point precision:
                // Create any triangulation (simple barycentric tests with inclusive error margin to create
                // cavity, insert point and connect). Then iterate by checking a tetrahedron with all neighbours,
                // calculate and add the solid angle of their unique vertex and compare with the summed
                // solid angle of all possible edge flips, take the minimum, if the difference between the
                // current orientation and minimum is larger than some error margin.
                // Continue iterating until no flips are made.
                throw "It is time, John Codeman, to gaze upon the abyss once more";
            }
            // This naive test should always come out positive once when comparing to itself
            // So we only return true for 1 or less
            return match_count < 2;
        });
        // Finally, for every unique triangle construct a tetrahedron
        // where one vertex lies on the subdividable point
        for (const auto& triangle : unique_triangle_list)
        {
            tetrahedrons.push_back({ { point, triangle[0], triangle[1], triangle[2] } });
        }
    }
    // Prune any tetrahedrons that are attached to the original bounding box
    // A 4 level deep nested loop looks scary, but it seems to run fine
    for (int i = 0; i < tetrahedrons.size(); i++)
    {
        bool vertex_match = false;
        for (const auto& vertex : tetrahedrons[i].vertices)
        {
            for (const auto& bounding_tetrahedron : kBoundingTetrahedrons)
            {
                for (const auto& bounding_vertex : bounding_tetrahedron.vertices)
                {
                    if (vertex == bounding_vertex)
                    {
                        vertex_match = true;
                    }
                }
            }
        }
        if (vertex_match)
        {
            tetrahedrons.erase(tetrahedrons.begin() + i);
            // We just erased, so the next element is actually at i
            i--;
        }
    }

    return tetrahedrons;
}

void RadianceTransferBaker::BakeProbeNetworkInnerCells(const std::vector<Tetrahedron>& tetrahedrons)
{
    // Create a probe search cell for each tetrahedron
    for (const auto& tetrahedron : tetrahedrons)
    {
        LightSector::ProbeSearchCell cell;
        cell.probe_vertices = { LightSector::INVALID_ID, LightSector::INVALID_ID, LightSector::INVALID_ID, LightSector::INVALID_ID };
        cell.neighbours = { LightSector::INVALID_ID, LightSector::INVALID_ID, LightSector::INVALID_ID, LightSector::INVALID_ID };
        // Translate vertex positions to matching probe positions by ID
        for (int i = 0; i < tetrahedron.vertices.size(); i++)
        {
            for (const auto& probe : probes_)
            {
                if (probe.pos == tetrahedron.vertices[i])
                {
                    cell.probe_vertices[i] = probe.id;
                    break;
                }
            }
        }
        // Check to ensure all vertices found their matching probe
        for (const auto& probe_id : cell.probe_vertices)
        {
            if (probe_id == LightSector::INVALID_ID)
            {
                throw "Could not find probe matching triangulated vertex";
            }
        }
        // Sort by ID to make face comparisons easier during neighbour search
        std::sort(cell.probe_vertices.begin(), cell.probe_vertices.end());
        probe_network_.push_back(cell);
    }
}

void RadianceTransferBaker::BakeProbeNetworkInnerNeighbours()
{
    // Find neighbouring indices
    // For each cell
    for (int self_id = 0; self_id < probe_network_.size(); self_id++)
    {
        auto& self = probe_network_[self_id];
        // For each face
        for (int self_face = 0; self_face < kProbeNetworkFaces; self_face++)
        {
            // This was already calculated while being determined as an "other" face, skip
            if (self.neighbours[self_face] != LightSector::INVALID_ID)
            {
                continue;
            }
            std::array<int, 3> self_probes;
            switch (self_face)
            {
            case LightSector::FACE_012:
                self_probes = { self.probe_vertices[0], self.probe_vertices[1], self.probe_vertices[2] };
                break;
            case LightSector::FACE_023:
                self_probes = { self.probe_vertices[0], self.probe_vertices[2], self.probe_vertices[3] };
                break;
            case LightSector::FACE_013:
                self_probes = { self.probe_vertices[0], self.probe_vertices[1], self.probe_vertices[3] };
                break;
            case LightSector::FACE_123:
                self_probes = { self.probe_vertices[1], self.probe_vertices[2], self.probe_vertices[3] };
                break;
            default:
                throw "Hit impossible face statement";
            }
            // Check against every other cell & face
            for (int other_id = 0; other_id < probe_network_.size(); other_id++)
            {
                // Don't compare with self, skip
                if (self_id == other_id)
                {
                    continue;
                }
                auto& other = probe_network_[other_id];
                for (int other_face = 0; other_face < kProbeNetworkFaces; other_face++)
                {
                    std::array<int, 3> other_probes;
                    switch (other_face)
                    {
                    case LightSector::FACE_012:
                        other_probes = { other.probe_vertices[0], other.probe_vertices[1], other.probe_vertices[2] };
                        break;
                    case LightSector::FACE_023:
                        other_probes = { other.probe_vertices[0], other.probe_vertices[2], other.probe_vertices[3] };
                        break;
                    case LightSector::FACE_013:
                        other_probes = { other.probe_vertices[0], other.probe_vertices[1], other.probe_vertices[3] };
                        break;
                    case LightSector::FACE_123:
                        other_probes = { other.probe_vertices[1], other.probe_vertices[2], other.probe_vertices[3] };
                        break;
                    default:
                        throw "Hit impossible face statement";
                    }
                    // Inner cells have sorted probe vertices so a straight comparison works fine
                    if (self_probes == other_probes)
                    {
                        self.neighbours[self_face] = other_id;
                        other.neighbours[other_face] = self_id;
                    }
                }
            }
        }
    }
}

void RadianceTransferBaker::BakeProbeNetworkOuterCells()
{
    const std::size_t outer_cell_start = probe_network_.size();

    // Use an interator since we are modifying the list as we go
    for (int i = 0; i < outer_cell_start; i++)
    {
        auto& cell = probe_network_[i];
        for (const auto& face : { LightSector::FACE_123, LightSector::FACE_023, LightSector::FACE_013, LightSector::FACE_012 })
        {
            // Neighbour is an outer cell
            if (cell.neighbours[face] == -1)
            {
                LightSector::ProbeSearchCell outer_cell;
                outer_cell.neighbours[LightSector::FACE] = i;
                outer_cell.neighbours[LightSector::EDGE_01] = LightSector::INVALID_ID;
                outer_cell.neighbours[LightSector::EDGE_02] = LightSector::INVALID_ID;
                outer_cell.neighbours[LightSector::EDGE_12] = LightSector::INVALID_ID;
                // Used to ensure the winding on the outer cell's triangle is correct
                int skipped_probe = -1;
                switch (face)
                {
                case LightSector::FACE_123:
                    outer_cell.probe_vertices = { cell.probe_vertices[1], cell.probe_vertices[2], cell.probe_vertices[3], LightSector::INVALID_ID };
                    skipped_probe = cell.probe_vertices[0];
                    break;
                case LightSector::FACE_023:
                    outer_cell.probe_vertices = { cell.probe_vertices[0], cell.probe_vertices[2], cell.probe_vertices[3], LightSector::INVALID_ID };
                    skipped_probe = cell.probe_vertices[1];
                    break;
                case LightSector::FACE_013:
                    outer_cell.probe_vertices = { cell.probe_vertices[0], cell.probe_vertices[1], cell.probe_vertices[3], LightSector::INVALID_ID };
                    skipped_probe = cell.probe_vertices[2];
                    break;
                case LightSector::FACE_012:
                    outer_cell.probe_vertices = { cell.probe_vertices[0], cell.probe_vertices[1], cell.probe_vertices[2], LightSector::INVALID_ID };
                    skipped_probe = cell.probe_vertices[3];
                    break;
                default:
                    throw "Impossible case statement during outer cell probe network bake";
                }
                // Ensure that the probe vertices are wound such that the cell's normal points outward, away from the tetrahedral mesh
                std::array<Vector3, 3> pos = { probes_[outer_cell.probe_vertices[0]].pos,
                                               probes_[outer_cell.probe_vertices[1]].pos,
                                               probes_[outer_cell.probe_vertices[2]].pos };
                // Uses the non-copied probe as a reference point as it should always be inside the mesh
                Vector3 inner_pos = probes_[skipped_probe].pos;
                Vector3 plane_normal = VectorCross(pos[1] - pos[0], pos[2] - pos[0]);
                // If the cosine of the normal and inner pointing ray is positive, then the winding is incorrect
                if (VectorDot(plane_normal, inner_pos - pos[0]) > 0.0f)
                {
                    std::iter_swap(outer_cell.probe_vertices.begin() + 1, outer_cell.probe_vertices.begin() + 2);
                }
                // Link the inner cell to the new outer cell
                cell.neighbours[face] = static_cast<int>(probe_network_.size());
                // Push the outer cell onto the network
                probe_network_.push_back(outer_cell);
            }
        }
    }
}

void RadianceTransferBaker::BakeProbeNetworkOuterNeighbours()
{
    // Find neighbouring indices
    // For each cell
    for (int self_id = 0; self_id < probe_network_.size(); self_id++)
    {
        auto& self = probe_network_[self_id];
        if (!LightSector::IsOuterProbeSearchCell(self))
        {
            continue;
        }
        // For each edge
        for (int self_edge = 0; self_edge < kProbeNetworkEdges; self_edge++)
        {
            // This was already calculated while being determined as an "other" edge, skip
            if (self.neighbours[self_edge] != LightSector::INVALID_ID)
            {
                continue;
            }
            std::array<int, 2> self_probes;
            switch (self_edge)
            {
            case LightSector::EDGE_12:
                self_probes = { self.probe_vertices[1], self.probe_vertices[2] };
                break;
            case LightSector::EDGE_02:
                self_probes = { self.probe_vertices[0], self.probe_vertices[2] };
                break;
            case LightSector::EDGE_01:
                self_probes = { self.probe_vertices[0], self.probe_vertices[1] };
                break;
            default:
                throw "Hit impossible face statement";
            }
            // Check against every other cell & edge
            for (int other_id = 0; other_id < probe_network_.size(); other_id++)
            {
                auto& other = probe_network_[other_id];
                // Don't compare with self and don't use inner cells, skip
                if (self_id == other_id || !LightSector::IsOuterProbeSearchCell(other))
                {
                    continue;
                }
                for (int other_edge = 0; other_edge < kProbeNetworkEdges; other_edge++)
                {
                    std::array<int, 2> other_probes;
                    switch (other_edge)
                    {
                    case LightSector::EDGE_12:
                        other_probes = { other.probe_vertices[1], other.probe_vertices[2] };
                        break;
                    case LightSector::EDGE_02:
                        other_probes = { other.probe_vertices[0], other.probe_vertices[2] };
                        break;
                    case LightSector::EDGE_01:
                        other_probes = { other.probe_vertices[0], other.probe_vertices[1] };
                        break;
                    default:
                        throw "Hit impossible face statement";
                    }
                    // Outer cell vertices cannot be sorted so we compare them like this
                    if (std::min(self_probes[0], self_probes[1]) == std::min(other_probes[0], other_probes[1]) &&
                        std::max(self_probes[0], self_probes[1]) == std::max(other_probes[0], other_probes[1]))
                    {
                        self.neighbours[self_edge] = other_id;
                        other.neighbours[other_edge] = self_id;
                    }
                }
            }
        }
    }
}

void RadianceTransferBaker::BakeProbeNetworkHullNormals()
{
    // We only need to store normals for probes that exist on the network hull.
    // Meaning this will have many empty values, but it's worth the instant
    // lookup for the temporary cost of some memory
    hull_normals_.resize(probes_.size());

    // This is implemented in a way that should hopefully never create a hull_normal
    // that has an angle between any of its connected face_normals greater than pi/2
    for (const auto& cell : probe_network_)
    {
        // Find every outer cell
        if (!LightSector::IsOuterProbeSearchCell(cell))
        {
            continue;
        }
        std::array<Vector3, 3> face = { probes_[cell.probe_vertices[0]].pos, probes_[cell.probe_vertices[1]].pos, probes_[cell.probe_vertices[2]].pos };
        Vector3 face_normal = VectorNormalize(VectorCross(face[1] - face[0], face[2] - face[0]));
        // Iterate over every vertex in each outer cell
        for (const auto& edge : { LightSector::EDGE_12, LightSector::EDGE_02, LightSector::EDGE_01 })
        {
            Vector3 edge1, edge2;
            int probe_id;
            switch (edge)
            {
            case LightSector::EDGE_12:
                edge1 = face[1] - face[0];
                edge2 = face[2] - face[0];
                probe_id = cell.probe_vertices[0];
                break;
            case LightSector::EDGE_02:
                edge1 = face[0] - face[1];
                edge2 = face[2] - face[1];
                probe_id = cell.probe_vertices[1];
                break;
            case LightSector::EDGE_01:
                edge1 = face[0] - face[2];
                edge2 = face[1] - face[2];
                probe_id = cell.probe_vertices[2];
                break;
            default:
                throw "Impossible case statement for LightSector edge iteration";
                break;
            }
            float angle_weight = acos(VectorDot(VectorNormalize(edge1), VectorNormalize(edge2)));
            // For each vertex, add the cell's normal weighted by the angle of the vertex's 2 connecting edges
            hull_normals_[probe_id] += face_normal * angle_weight;
        }
    }
    // Afterwards normalize each hull normal
    for (auto& normal : hull_normals_)
    {
        if (VectorLength(normal) > 0.0f)
        {
            normal = VectorNormalize(normal);
        }
    }
}

void RadianceTransferBaker::BakeProbeNetworkConvererters()
{
    // Build barycentric conversion matrices
    for (auto& cell : probe_network_)
    {
        // For inner cells we can build a matrix that takes as input
        // the world position to test subtracted by the 0th vertex (world_pos - vertices[0].pos)
        // and outputs a Vector3 of the barycentric coordinates for the tetrahedron
        // The last coordinate is found with the calculation 1 - b.x - b.y - b.z
        if (!LightSector::IsOuterProbeSearchCell(cell))
        {
            // Formula taken from:
            // https://en.wikipedia.org/wiki/Barycentric_coordinate_system
            // Described as:
            // [b1] = T^(-1) * (p - r0)
            // [b2]
            // b3 = 1 - b1 - b2
            // Where b1, b2, b3 are the barycentric coordinates
            // p is the point to convert
            // r0, r1, r2 are the points of a triangle
            // T = [r1.x - r0.x, r2.x - r0.x]
            //     [r1.y - r0.y, r2.y - r0.y]
            // This is meant for triangles, as we use tetrahedrons our matrix is 3x3 instead

            // Matrix that will hold the inverse of our barycentric converter
            Matrix T;
            // Create point deltas with the first vertex as the origin
            Vector3 origin = probes_[cell.probe_vertices[0]].pos;
            Vector3 d1 = probes_[cell.probe_vertices[1]].pos - origin;
            Vector3 d2 = probes_[cell.probe_vertices[2]].pos - origin;
            Vector3 d3 = probes_[cell.probe_vertices[3]].pos - origin;
            // We are use row-major matrices, so we instead use the transpose
            T.m[0][0] = d1.x; T.m[0][1] = d1.y; T.m[0][2] = d1.z; T.m[0][3] = 0;
            T.m[1][0] = d2.x; T.m[1][1] = d2.y; T.m[1][2] = d2.z; T.m[1][3] = 0;
            T.m[2][0] = d3.x; T.m[2][1] = d3.y; T.m[2][2] = d3.z; T.m[2][3] = 0;
            T.m[3][0] = 0;    T.m[3][1] = 0;    T.m[3][2] = 0;    T.m[3][3] = 1;
            // Store the inverse
            cell.barycentric_converter = MatrixInverse(T);
        }
        // For outer cells things are more complicated requiring more calculations
        // so instead the matrix is used to store necessary data in an arbitrary format.
        // While messy, and not very safe or clear, things are done this way to allow
        // cells to share the same data container on the GPU
        else
        {
            // The first 3 rows of the conversion matrix will store the hull normal to the matching vertex
            // scaled by the inverse of the angle between the hull normal and face normal. This gives the normal
            // a length such that the vertex + normal would be a position with a distance to the hull plane of 1.
            // This is useful later when calculating barycentric coordinates by way of extruding a triangle that
            // has a plane parallel to the hull plane as well as has each vertex lying on the ray of each hull normal
            Matrix matrix = MatrixIdentity();
            std::array<Vector3, 3> face = { probes_[cell.probe_vertices[0]].pos, probes_[cell.probe_vertices[1]].pos, probes_[cell.probe_vertices[2]].pos };
            Vector3 face_normal = VectorNormalize(VectorCross(face[1] - face[0], face[2] - face[0]));
            for (int i = 0; i < 3; i++)
            {
                Vector3 hull_normal = hull_normals_[cell.probe_vertices[i]];
                float cosine_angle = VectorDot(face_normal, hull_normal);
                if (cosine_angle < 0.0f)
                {
                    throw "Generated probe network hull normal has angle greater than pi/2 with connecting face";
                }
                hull_normal *= 1.0f / cosine_angle;
                matrix.m[i][0] = hull_normal.x;
                matrix.m[i][1] = hull_normal.y;
                matrix.m[i][2] = hull_normal.z;
            }
            cell.barycentric_converter = matrix;
        }
    }
}
} // namespace stage
} // namespace pipeline
} // namespace blons
