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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTSECTOR_LIGHTSECTOR_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTSECTOR_LIGHTSECTOR_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/pipeline/stage/shadow.h>
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
    struct Probe
    {
        const int id;
        Vector3 pos;
        AmbientCube irradiance;
        SHCoeffs3 sh_sky_visibility;
        int brick_factor_range_start;
        int brick_factor_count;
    };

    // Uses Delaunay Triangulation for finding and interpolating probes
    // Tetrahedron with ID of neighbouring tetrahedrons indexed
    struct ProbeSearchCell
    {
        std::array<int, 4> probe_vertices; // List of probe IDs
        std::array<int, 4> neighbours; // Neighbouring tetrahedrons by face
        Matrix barycentric_converter; // Converts world coords to barycentric with (wpos - vertex[0].pos) * mat
    };

    enum ProbeSearchValue
    {
        INVALID_ID = -1
    };

    enum ProbeSearchFaceIndex
    {
        FACE_012,
        FACE_013,
        FACE_023,
        FACE_123
    };

    // Uniformly sized voxel face
    struct Surfel
    {
        int nearest_probe_id;
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
        float brick_weights[6];
    };

public:
    LightSector();
    ~LightSector() {}

    void BakeRadianceTransfer(const Scene& scene);

    bool Relight(const Scene& scene, const Shadow& shadow, Matrix light_vp_matrix);

    const std::vector<Probe>& probes() const;
    const ShaderDataResource* probe_shader_data() const;
    const std::vector<ProbeSearchCell>& probe_network() const;
    const std::vector<Surfel>& surfels() const;
    const ShaderDataResource* surfel_shader_data() const;
    const std::vector<SurfelBrick>& surfel_bricks() const;
    const ShaderDataResource* surfel_brick_shader_data() const;
    const std::vector<SurfelBrickFactor>& surfel_brick_factors() const;
    const ShaderDataResource* surfel_brick_factor_shader_data() const;

private:
    std::vector<Probe> probes_;
    std::vector<ProbeSearchCell> probe_network_;
    std::vector<Surfel> surfels_;
    std::vector<SurfelBrick> surfel_bricks_;
    std::vector<SurfelBrickFactor> surfel_brick_factors_;
    std::unique_ptr<ComputeShader> probe_relight_shader_;
    std::unique_ptr<ComputeShader> surfel_brick_relight_shader_;
    std::unique_ptr<ShaderData<Probe>> probe_shader_data_;
    std::unique_ptr<ShaderData<Surfel>> surfel_shader_data_;
    std::unique_ptr<ShaderData<SurfelBrick>> surfel_brick_shader_data_;
    std::unique_ptr<ShaderData<SurfelBrickFactor>> surfel_brick_factor_shader_data_;
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

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTSECTOR_LIGHTSECTOR_H_
