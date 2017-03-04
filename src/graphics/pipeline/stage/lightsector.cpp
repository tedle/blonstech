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

#include <blons/graphics/pipeline/stage/lightsector.h>

// Includes
#include <algorithm>
#include <array>
#include <numeric>
// Public Includes
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace
{
auto cvar_constant_ambient = console::RegisterVariable("light:constant-ambient", 1500.0f);

// Hard coded distance clipping as graphics option values are tuned for performance
const Matrix kCubeFaceProjection = MatrixPerspective(kPi / 2.0f, 1.0f, 0.1f, 100.0f);
const std::vector<AxisAlignedNormal> kFaceOrder = { NEGATIVE_Z, POSITIVE_X, POSITIVE_Z, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y };
Vector3 FaceRotation(AxisAlignedNormal face)
{
    units::world pitch = 0.0f;
    units::world yaw = 0.0f;
    switch (face)
    {
    case NEGATIVE_Z:
        break;
    case POSITIVE_X:
        yaw = -kPi / 2.0f;
        break;
    case POSITIVE_Z:
        yaw = kPi;
        break;
    case NEGATIVE_X:
        yaw = kPi / 2.0f;
        break;
    case POSITIVE_Y:
        pitch = kPi / 2.0f;
        break;
    case NEGATIVE_Y:
        pitch = -kPi / 2.0f;
        break;
    default:
        throw "Impossible case statment reached during face selection";
    }
    return Vector3(pitch, yaw, 0.0f);
}

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

namespace temp
{
void GenerateTestProbe(std::vector<LightSector::Probe>* probes)
{
    LightSector::Probe probe{ static_cast<int>(probes->size()), Vector3(0.0f, 3.0f, 0.0f) };
    probe.brickfactor_range_start = 0;
    probe.brickfactor_count = 0;
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
                probe.brickfactor_range_start = 0;
                probe.brickfactor_count = 0;
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
                probe.brickfactor_range_start = 0;
                probe.brickfactor_count = 0;
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
                probe.brickfactor_range_start = 0;
                probe.brickfactor_count = 0;
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
                probe.brickfactor_range_start = 0;
                probe.brickfactor_count = 0;
                probes->push_back(probe);
            }
        }
    }
    for (int x = 0; x < 6; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            LightSector::Probe probe { static_cast<int>(probes->size()), Vector3(-10.0f + x * 4.0f, y * 3.0f + 11.0f, 0) };
            probe.brickfactor_range_start = 0;
            probe.brickfactor_count = 0;
            probes->push_back(probe);
        }
    }
}
} // namespace temp

LightSector::LightSector()
{
    temp::GenerateOldSponzaProbes(&probes_);
    // Initialize shader buffer to fit all probes in
    probe_shader_data_.reset(new ShaderData<LightSector::Probe>(nullptr, probes_.size()));

    // Setup shader for generating environment maps to build surfel and sky visibility data
    ShaderAttributeList env_map_inputs;
    env_map_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    env_map_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    env_map_inputs.push_back(ShaderAttribute(ShaderAttributeIndex::NORMAL, "input_norm"));
    env_map_inputs.push_back(ShaderAttribute(TANGENT, "input_tan"));
    env_map_inputs.push_back(ShaderAttribute(BITANGENT, "input_bitan"));
    environment_map_shader_.reset(new Shader("shaders/probe-env-map.vert.glsl", "shaders/probe-env-map.frag.glsl", env_map_inputs));

    environment_maps_.reset(new Framebuffer(kProbeMapSize * 6, kProbeMapSize * static_cast<units::pixel>(probes_.size()),
                                            { { TextureType::R8G8B8A8, TextureType::RAW, TextureType::NEAREST, TextureType::CLAMP },   // albedo + sky vis
                                              { TextureType::R8G8B8,   TextureType::RAW, TextureType::NEAREST, TextureType::CLAMP } }, // normal
                                            true));

    // Relight compute shaders to be run every frame
    surfel_brick_relight_shader_.reset(new ComputeShader("shaders/surfelbrick-relight.comp.glsl"));
    probe_relight_shader_.reset(new ComputeShader("shaders/probe-relight.comp.glsl"));
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
        !surfel_brick_relight_shader_->SetInput("surfel_buffer", surfel_shader_data()) ||
        !surfel_brick_relight_shader_->SetInput("surfel_brick_buffer", surfel_brick_shader_data()))
    {
        return false;
    }
    surfel_brick_relight_shader_->Run(static_cast<unsigned int>(surfel_bricks_.size()), 1, 1);

    // Iterate over every probe, building an irradiance term from sky light and any visible surfel bricks
    if (!probe_relight_shader_->SetInput("probe_buffer", probe_shader_data()) ||
        !probe_relight_shader_->SetInput("sky_luminance", scene.sky_luminance) ||
        !probe_relight_shader_->SetInput("temp_ambient", cvar_constant_ambient->to<float>()) ||
        !probe_relight_shader_->SetInput("sh_sky_colour.r", scene.sky_box.r.coeffs, 9) ||
        !probe_relight_shader_->SetInput("sh_sky_colour.g", scene.sky_box.g.coeffs, 9) ||
        !probe_relight_shader_->SetInput("sh_sky_colour.b", scene.sky_box.b.coeffs, 9))
    {
        return false;
    }
    probe_relight_shader_->Run(static_cast<unsigned int>(probes_.size()), 1, 1);
    return true;
}

void LightSector::BakeRadianceTransfer(const Scene& scene)
{
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
    // Update shader buffer with generated radiance data
    probe_shader_data_->set_value(probes_.data());
    surfel_shader_data_.reset(new ShaderData<LightSector::Surfel>(surfels_.data(), surfels_.size()));
    surfel_brick_shader_data_.reset(new ShaderData<LightSector::SurfelBrick>(surfel_bricks_.data(), surfel_bricks_.size()));
    surfel_brick_factor_shader_data_.reset(new ShaderData<LightSector::SurfelBrickFactor>(surfel_brick_factors_.data(), surfel_brick_factors_.size()));
}

const TextureResource* LightSector::output(Output buffer) const
{
    switch (buffer)
    {
    case ENV_MAPS_ALBEDO:
        return environment_maps_->textures()[0];
        break;
    case ENV_MAPS_NORMAL:
        return environment_maps_->textures()[1];
        break;
    default:
        return nullptr;
    }
}

const std::vector<LightSector::Probe>& LightSector::probes() const
{
    return probes_;
}

const ShaderDataResource* LightSector::probe_shader_data() const
{
    return probe_shader_data_->data();
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

// This function only exists to help compartmentalize the long process of PRT baking
void LightSector::BakeEnvironmentMaps(const Scene& scene)
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

    // Build up a buffer of unique face data used for instanced rendering
    for (const auto& probe : probes_)
    {
        cube_view.set_pos(probe.pos.x, probe.pos.y, probe.pos.z);
        int face_index = 0;
        for (const auto& face : kFaceOrder)
        {
            PerFaceData face_data;
            Vector3 rot = FaceRotation(face);
            cube_view.set_rot(rot.x, rot.y, rot.z);
            face_data.vp_matrix = cube_view.view_matrix() * kCubeFaceProjection;
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

void LightSector::GatherProbeSamples(std::vector<SurfelSample>* surfel_samples, std::vector<SkyVisSample>* sky_samples)
{
    // Pre-allocate sample containers
    surfel_samples->reserve(kProbeMapSize * kProbeMapSize * 6 * probes_.size());
    sky_samples->reserve(kProbeMapSize * kProbeMapSize * 6 * probes_.size());

    // Retrieve textures and pixel spacing in memory
    auto albedo_tex = render::context()->GetTextureData(environment_maps_->textures()[0]);
    auto normal_tex = render::context()->GetTextureData(environment_maps_->textures()[1]);
    auto depth_tex = render::context()->GetTextureData(environment_maps_->depth());
    std::size_t albedo_pixel_size = albedo_tex.bits_per_pixel() / 8;
    std::size_t normal_pixel_size = normal_tex.bits_per_pixel() / 8;
    std::size_t depth_pixel_size = depth_tex.bits_per_pixel() / 8;

    // Iterate over each face of each probe and generate samples
    for (const auto& probe : probes_)
    {
        int face_index = 0;
        for (const auto& face : kFaceOrder)
        {
            // Reconstruct camera rotation that was used to render scene for this face
            Vector3 rot = FaceRotation(face);
            Camera cube_view;
            cube_view.set_pos(0, 0, 0);
            cube_view.set_rot(rot.x, rot.y, rot.z);
            // Since a view matrix rotates things in the opposite of the direction given
            // We use the inverse for determining the normal of the sphere at a given texel
            Matrix sphere_rotation_matrix = MatrixInverse(cube_view.view_matrix());
            // Reconstruct full camera rotation and position that was used to render scene for this face
            // Used for reconstructing the world space position at a given texel, same as in deferred rendering
            cube_view.set_pos(probe.pos.x, probe.pos.y, probe.pos.z);
            Matrix inverse_vp_matrix = MatrixInverse(cube_view.view_matrix() * kCubeFaceProjection);

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
                        Surfel surfel;
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

void LightSector::BakeSurfelClusters(const std::vector<SurfelSample>& samples)
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

LightSector::SurfelCluster LightSector::ClusterSurfelData(const std::vector<SurfelSample>& samples)
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

LightSector::BrickCluster LightSector::ClusterBrickData(SurfelCluster* surfel_data)
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

void LightSector::GenerateBrickWeights(const std::vector<BakeBrick>& bricks)
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
            std::copy(std::begin(probe.second), std::end(probe.second), std::begin(f.factor.brick_weight));
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
        if (probe.brickfactor_count == 0)
        {
            probe.brickfactor_range_start = i;
        }
        probe.brickfactor_count++;
        // Transfer to contiguous memory
        surfel_brick_factors_.push_back(factor.factor);
    }
}

void LightSector::NormalizeBrickWeights()
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
        auto start = surfel_brick_factors_.begin() + probe.brickfactor_range_start;
        auto end = start + probe.brickfactor_count;
        // Apply normalization factor
        std::transform(start, end, start,
        [&](auto bf)
        {
            for (int face = 0; face < 6; face++)
            {
                bf.brick_weight[face] /= normalization_factor;
            }
            return bf;
        });
    }
}

void LightSector::BakeSkyCoefficients(const std::vector<SkyVisSample>& samples)
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
} // namespace stage
} // namespace pipeline
} // namespace blons
