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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTSECTOR_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTSECTOR_H_

// Includes
#include <unordered_map>
// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>
#include <blons/graphics/render/computeshader.h>
#include <blons/graphics/render/shaderdata.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Manages light probes used to calculate indirect illumination at
/// runtime with the use of precomputed radiance transfer
////////////////////////////////////////////////////////////////////////////////
class LightSector
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which output of the stage to retrieve
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        ENV_MAPS_ALBEDO, ///< Cached albedo environment maps
        ENV_MAPS_NORMAL  ///< Cached normal environment maps
    };

    struct Probe
    {
        const int id;
        Vector3 pos;
        AmbientCube irradiance;
        SHCoeffs3 sh_sky_visibility;
        int brickfactor_range_start;
        int brickfactor_count;
    };

    // Uniformly sized voxel face
    struct Surfel
    {
        // TODO: Add nearest probe for bounce lighting
        int nearest_probe;
        Vector3 pos;
        Vector3 normal;
        Vector3 albedo;
        Vector3 radiance;
    };

    struct SurfelBrick
    {
        int surfel_range_start;
        int surfel_count;
        Vector3 radiance;
    };

    struct SurfelBrickFactor
    {
        int brick_id;
        float brick_weight[6];
    };

public:
    LightSector();
    ~LightSector() {}

    void BakeRadianceTransfer(const Scene& scene);

    bool Relight(const Scene& scene);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer Lighting::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;

    const std::vector<Probe>& probes() const;
    const ShaderDataResource* probe_shader_data() const;
    const std::vector<Surfel>& surfels() const;
    const ShaderDataResource* surfel_shader_data() const;

private:
    // Used in PRT baking
    struct SurfelSample
    {
        LightSector::Surfel surfel;
        int parent_probe;
        float parent_probe_weights[6];
    };
    struct SkyVisSample
    {
        Vector2 uv;
        Vector3 normal;
        float visibility;
        int parent_probe;
    };
    struct BakeSurfel
    {
        LightSector::Surfel surfel;
        int sample_count;
        struct ParentProbeData
        {
            int id;
            float weights[6];
        };
        std::vector<ParentProbeData> parent_probes;
    };
    struct BakeBrick
    {
        LightSector::SurfelBrick brick;
        std::vector<BakeSurfel> surfels;
    };
    struct BakeBrickFactor
    {
        SurfelBrickFactor factor;
        int parent_probe;
    };
    struct SurfelIndex
    {
        int x, y, z;
        AxisAlignedNormal direction;
        struct HashFunc { unsigned int operator()(const SurfelIndex& s) const { return FastHash(&s, sizeof(SurfelIndex)); } };
        struct CompFunc { bool operator()(const SurfelIndex& a, const SurfelIndex& b) const { return std::tie(a.x, a.y, a.z, a.direction) == std::tie(b.x, b.y, b.z, b.direction); } };
    };
    using SurfelCluster = std::unordered_map<SurfelIndex, BakeSurfel, SurfelIndex::HashFunc, SurfelIndex::CompFunc>;
    using BrickCluster = std::unordered_map<SurfelIndex, BakeBrick, SurfelIndex::HashFunc, SurfelIndex::CompFunc>;

    void BakeEnvironmentMaps(const Scene& scene);
    void GatherProbeSamples(std::vector<SurfelSample>* surfel_samples, std::vector<SkyVisSample>* sky_samples);
    void BakeSurfelClusters(const std::vector<SurfelSample>& samples);
        SurfelCluster ClusterSurfelData(const std::vector<SurfelSample>& samples);
        BrickCluster ClusterBrickData(SurfelCluster* surfel_data);
        void GenerateBrickWeights(const std::vector<BakeBrick>& bricks);
        void NormalizeBrickWeights();
    void BakeSkyCoefficients(const std::vector<SkyVisSample>& samples);

    std::vector<Probe> probes_;
    std::vector<Surfel> surfels_;
    std::vector<SurfelBrick> surfel_bricks_;
    std::vector<SurfelBrickFactor> surfel_brick_factors_;
    std::unique_ptr<Framebuffer> environment_maps_;
    std::unique_ptr<Shader> environment_map_shader_;
    std::unique_ptr<ComputeShader> probe_relight_shader_;
    std::unique_ptr<ShaderData<Probe>> probe_shader_data_;
    std::unique_ptr<ShaderData<Surfel>> surfel_shader_data_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

/// \NEEDS_DOCUMENTATION
////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::LightSector
/// \ingroup pipeline
///
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTSECTOR_H_
