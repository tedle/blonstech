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
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Contains a light probe used for computing ambient diffuse lighting
    /// in real time
    ///
    /// \ingroup gpu
    ////////////////////////////////////////////////////////////////////////////////
    struct Probe
    {
        const int id;                 ///< Numerical ID of probe, ranges from [0,n-1]
        Vector3 pos;                  ///< World space position of probe
        AmbientCube irradiance;       ///< Ambient cube that caches indirect lighting after LightSector::Relight
        SHCoeffs3 sh_sky_visibility;  ///< Spherical harmonic coefficients storing probe's sky visibility for IBL diffuse term
        int brick_factor_range_start; ///< Starting index of indirect radiance bricks associated with this probe
        int brick_factor_count;       ///< Number of indirect radiance bricks associated with this probe
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Defines a tetrahedral cell with 4 Probe%s at each vertex. A group of
    /// these cells composes a Delaunay triangulated network used for interpolation
    /// between Probe%s. Can also define a triangular outer cell, distinguished by
    /// a missing 4th vertex used for interpolation outside of the triangulated mesh
    ///
    /// \ingroup gpu
    ////////////////////////////////////////////////////////////////////////////////
    struct ProbeSearchCell
    {
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief List of 4 Probe IDs that compose the tetrahedral cell
        ////////////////////////////////////////////////////////////////////////////////
        std::array<int, 4> probe_vertices;
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief List of 4 ProbeSearchCell%s neighbouring each face of the cell.
        /// Indexed by ProbeSearchInnerIndex and ProbeSearchOuterIndex
        ////////////////////////////////////////////////////////////////////////////////
        std::array<int, 4> neighbours;
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Contains precomputed data for calculating the barycentric coordinates
        /// of a world space position relative to a ProbeSearchCell. For cells within
        /// the triangulation this holds a simple 3x3 matrix that can calculate 3 of the
        /// 4 barycentric coordinates with a simple `(wpos - vertex[0].pos) * mat`
        /// multiplication. For cells outside the triangulation's hull this Matrix
        /// contains 3 separate vectors defining the normal of each outer vertex scaled
        /// so that they end at a distance of 1 from the plane aligned to the outer
        /// cell's triangle.
        ////////////////////////////////////////////////////////////////////////////////
        Matrix barycentric_converter;
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief List of possible values for ProbeSearchCell vertices and neighbours
    /// beyond simple lookup indices
    ////////////////////////////////////////////////////////////////////////////////
    enum ProbeSearchValue
    {
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Default value for all vertices and neighbours. Likely indicates an
        /// oversight in the baking process. Can also be used to denote an outer
        /// ProbeSearchCell by marking the 4th vertex as INVALID_ID since outer cells
        /// only have 3 vertices.
        ////////////////////////////////////////////////////////////////////////////////
        INVALID_ID = -1
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief List of indices for a inner ProbeSearchCell neighbour array.
    /// Determines which neighbour is associated with which face. The index can also
    /// be figured out by using the missing vertex. For example finding the
    /// neighbour of the face composed of the vertices 0, 2, and 3 would be at index
    /// 1 since vertex 1 is not a part of that face.
    ////////////////////////////////////////////////////////////////////////////////
    enum ProbeSearchInnerIndex
    {
        FACE_123 = 0, ///< Face composed of vertices 1, 2, and 3
        FACE_023 = 1, ///< Face composed of vertices 0, 2, and 3
        FACE_013 = 2, ///< Face composed of vertices 0, 1, and 3
        FACE_012 = 3  ///< Face composed of vertices 0, 1, and 2
    };
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief List of indices for a outer ProbeSearchCell neighbour array.
    /// Determines which neighbour is associated with which edge. The index can also
    /// be figured out by using the missing vertex. For example finding the
    /// neighbour of the edge composed of the vertices 0 and 2 would be at index 1
    /// since vertex 1 is not a part of that edge. Since there is no 4th vertex,
    /// the edge associated with missing vertex 3 is instead of the face of the
    /// outer cell neighbouring the inside of the triangulated hull.
    ////////////////////////////////////////////////////////////////////////////////
    enum ProbeSearchOuterIndex
    {
        EDGE_12 = 0, ///< Edge composed of vertices 1 and 2
        EDGE_02 = 1, ///< Edge composed of vertices 0 and 2
        EDGE_01 = 2, ///< Edge composed of vertices 0 and 1
        FACE    = 3  ///< Face composed of all 3 vertices
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used for interpolating between Probe%s, weighting each Probe by their
    /// barycentric coordinates
    ////////////////////////////////////////////////////////////////////////////////
    struct ProbeWeight
    {
        int id;       ///< ID of the probe being interpolated
        float weight; ///< Weight of probe
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Carries the results of a Probe network interpolation search
    ////////////////////////////////////////////////////////////////////////////////
    using ProbeSearchWeights = std::array<ProbeWeight, 4>;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Contains a sample of geometry data used to simulate bounced indirect
    /// diffuse lighting in real time
    ///
    /// \ingroup gpu
    ////////////////////////////////////////////////////////////////////////////////
    struct Surfel
    {
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief ID of the nearest Probe. Used to crudely approximate infinite bounces
        /// by re-using the lighting from the previous frame
        ////////////////////////////////////////////////////////////////////////////////
        int nearest_probe_id;
        Vector3 pos;       ///< World space position
        Vector3 normal;    ///< World space normal
        Vector3 albedo;    ///< Albedo colour
        Vector3 radiance;  ///< Radiance gathered from sky ambient, Probe ambient, and nearby lights
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Contains a collection of Surfel%s to be averaged into a single
    /// lighting term for more efficient lookups during Probe relighting.
    ///
    /// \ingroup gpu
    ////////////////////////////////////////////////////////////////////////////////
    struct SurfelBrick
    {
        int surfel_range_start; ///< Starting index of Surfel%s associated with this brick
        int surfel_count;       ///< Number of Surfel%s associated with this brick
        Vector3 radiance;       ///< Calculated average radiance of associated Surfel%s
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Contains a list of weights for the 6 faces of an AmbientCube, each
    /// defining the influence of light from a given brick to a unique parent Probe.
    ///
    /// \ingroup gpu
    ////////////////////////////////////////////////////////////////////////////////
    struct SurfelBrickFactor
    {
        int brick_id;           ///< ID of SurfelBrick that affects the parent probe
        float brick_weights[6]; ///< Weights for each 6 faces of an ambient cube
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calculates indirect diffuse lighting term for a scene in real time.
    /// Requires precomputed radiance transfer information which can be baked after
    /// initialization.
    ////////////////////////////////////////////////////////////////////////////////
    LightSector();
    ~LightSector() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Bakes the precomputed radiance transfer data for a given scene
    ///
    /// \param scene Contains scene information for baking
    ////////////////////////////////////////////////////////////////////////////////
    void BakeRadianceTransfer(const Scene& scene);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Computes the lighting for all Probe%s contained in this sector
    ///
    /// \param scene Contains scene information for rendering
    /// \param shadow Handle to the shadow buffer pass performed earlier in the
    /// frame
    /// \param light_vp_matrix View projection matrix of the directional light
    /// providing shadow
    ////////////////////////////////////////////////////////////////////////////////
    bool Relight(const Scene& scene, const Shadow& shadow, Matrix light_vp_matrix);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines if a given ProbeSearchCell contains a volume outside the
    /// triangulated probe network hull
    ///
    /// \param cell Cell defining a volume inside or out of the probe network
    /// \return True if the cell is outside the probe network
    ////////////////////////////////////////////////////////////////////////////////
    static bool IsOuterProbeSearchCell(const ProbeSearchCell& cell);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Finds the interpolated weights of the Probe%s representing a confined
    /// cell for any given point in space
    ///
    /// \param point World space position to find associated Probe%s and weights
    /// \return List of 4 Probe%s and interpolated weights
    ////////////////////////////////////////////////////////////////////////////////
    ProbeSearchWeights FindProbeWeights(const Vector3& point) const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves list of Probe%s for this sector
    ///
    /// \return List of all Probe%s in this sector
    ////////////////////////////////////////////////////////////////////////////////
    const std::vector<Probe>& probes() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a handle to the Probe%s for this sector in GPU memory
    ///
    /// \return GPU handle for all Probe%s in this sector
    ////////////////////////////////////////////////////////////////////////////////
    const ShaderDataResource* probe_shader_data() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves list of ProbeSearchCell%s for this sector
    ///
    /// \return List of all ProbeSearchCell%s in this sector
    ////////////////////////////////////////////////////////////////////////////////
    const std::vector<ProbeSearchCell>& probe_network() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a handle to the ProbeSearchCell%s for this sector in GPU
    /// memory
    ///
    /// \return GPU handle for all ProbeSearchCell%s in this sector
    ////////////////////////////////////////////////////////////////////////////////
    const ShaderDataResource* probe_network_shader_data() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves list of Surfel%s for this sector
    ///
    /// \return List of all Surfel%s in this sector
    ////////////////////////////////////////////////////////////////////////////////
    const std::vector<Surfel>& surfels() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a handle to the Surfel%s for this sector in GPU memory
    ///
    /// \return GPU handle for all Surfel%s in this sector
    ////////////////////////////////////////////////////////////////////////////////
    const ShaderDataResource* surfel_shader_data() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves list of SurfelBrick%s for this sector
    ///
    /// \return List of all SurfelBrick%s in this sector
    ////////////////////////////////////////////////////////////////////////////////
    const std::vector<SurfelBrick>& surfel_bricks() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a handle to the SurfelBrick%s for this sector in GPU memory
    ///
    /// \return GPU handle for all SurfelBrick%s in this sector
    ////////////////////////////////////////////////////////////////////////////////
    const ShaderDataResource* surfel_brick_shader_data() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves list of SurfelBrickFactor%s for this sector
    ///
    /// \return List of all SurfelBrickFactor%s in this sector
    ////////////////////////////////////////////////////////////////////////////////
    const std::vector<SurfelBrickFactor>& surfel_brick_factors() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a handle to the SurfelBrickFactor%s for this sector in GPU
    /// memory
    ///
    /// \return GPU handle for all SurfelBrickFactor%s in this sector
    ////////////////////////////////////////////////////////////////////////////////
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
    std::unique_ptr<ShaderData<ProbeSearchCell>> probe_network_shader_data_;
    std::unique_ptr<ShaderData<Surfel>> surfel_shader_data_;
    std::unique_ptr<ShaderData<SurfelBrick>> surfel_brick_shader_data_;
    std::unique_ptr<ShaderData<SurfelBrickFactor>> surfel_brick_factor_shader_data_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::LightSector
/// \ingroup pipeline
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTSECTOR_LIGHTSECTOR_H_
