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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTSECTOR_RADIANCETRANSFERBAKER_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTSECTOR_RADIANCETRANSFERBAKER_H_

// Includes
#include <unordered_map>
// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/pipeline/stage/lightsector/lightsector.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
class RadianceTransferBaker
{
public:
    RadianceTransferBaker(const Scene& scene, const std::vector<LightSector::Probe>& probes);
    ~RadianceTransferBaker() {}

    const std::vector<LightSector::Probe>& probes() const;
    const std::vector<LightSector::ProbeSearchCell>& probe_network() const;
    const std::vector<LightSector::Surfel>& surfels() const;
    const std::vector<LightSector::SurfelBrick>& surfel_bricks() const;
    const std::vector<LightSector::SurfelBrickFactor>& surfel_brick_factors() const;

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
        LightSector::SurfelBrickFactor factor;
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

    // These functions only exists to help compartmentalize the long process of PRT baking
    void BakeEnvironmentMaps(const Scene& scene);
    void GatherProbeSamples(std::vector<SurfelSample>* surfel_samples, std::vector<SkyVisSample>* sky_samples);
    void BakeSurfelClusters(const std::vector<SurfelSample>& samples);
        SurfelCluster ClusterSurfelData(const std::vector<SurfelSample>& samples);
        BrickCluster ClusterBrickData(SurfelCluster* surfel_data);
        void GenerateBrickWeights(const std::vector<BakeBrick>& bricks);
        void NormalizeBrickWeights();
    void BakeSkyCoefficients(const std::vector<SkyVisSample>& samples);
    void BakeProbeNetwork();
        std::vector<Tetrahedron> TriangulateProbeNetwork();
        void BakeProbeNetworkInnerCells(const std::vector<Tetrahedron>& tetrahedrons);
        void BakeProbeNetworkInnerNeighbours();
        void BakeProbeNetworkOuterCells();
        void BakeProbeNetworkOuterNeighbours();
        void BakeProbeNetworkHullNormals();
        void BakeProbeNetworkConvererters();

    std::vector<LightSector::Probe> probes_;
    std::vector<LightSector::ProbeSearchCell> probe_network_;
    std::vector<LightSector::Surfel> surfels_;
    std::vector<LightSector::SurfelBrick> surfel_bricks_;
    std::vector<LightSector::SurfelBrickFactor> surfel_brick_factors_;
    std::unique_ptr<Framebuffer> environment_maps_;
    std::unique_ptr<Shader> environment_map_shader_;
    std::vector<Vector3> hull_normals_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons
#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTSECTOR_RADIANCETRANSFERBAKER_H_
