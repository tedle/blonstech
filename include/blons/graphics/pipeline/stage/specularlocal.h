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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_SPECULARLOCAL_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_SPECULARLOCAL_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/pipeline/stage/geometry.h>
#include <blons/graphics/pipeline/stage/shadow.h>
#include <blons/graphics/pipeline/stage/irradiancevolume.h>
#include <blons/graphics/texturecubemap.h>
#include <blons/graphics/render/computeshader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Bakes and relights a collection of local specular lighting probes
////////////////////////////////////////////////////////////////////////////////
class SpecularLocal
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which output of the stage to retrieve
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        ALBEDO, ///< G-Buffer albedo map
        NORMAL, ///< G-buffer normal map
        DEPTH,  ///< G-Buffer depth map
        LIGHT   ///< Filtered lighting information applied to the environment map
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new SpecularLocal stage
    ////////////////////////////////////////////////////////////////////////////////
    SpecularLocal();
    ~SpecularLocal() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Bakes the precomputed radiance transfer data for a given scene
    ///
    /// \param scene Contains scene information for baking
    ////////////////////////////////////////////////////////////////////////////////
    void BakeRadianceTransfer(const Scene& scene);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Relights and filters environment cubemaps
    ///
    /// \param scene Contains scene information for rendering
    /// \param shadow Handle to the shadow buffer pass performed earlier in the
    /// frame
    /// \param irradiance Handle to the irradiance volume pass performed earlier in
    /// the frame
    /// \param light_vp_matrix View-projection matrix of the direction light
    /// providing shadow
    ////////////////////////////////////////////////////////////////////////////////
    bool Relight(const Scene& scene, const Shadow& shadow, const IrradianceVolume& irradiance,
                 Matrix light_vp_matrix);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer Lighting::Output target to retrieve
    /// \param probe_id ID of the specular probe to retrieve outputs from
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer, std::size_t probe_id) const;

private:
    struct SpecularProbe
    {
        Vector3 pos;
        struct
        {
            std::unique_ptr<TextureCubemap> albedo;      ///< G-buffer albedo
            std::unique_ptr<TextureCubemap> normal;      ///< G-buffer normal
            std::unique_ptr<TextureCubemap> depth;       ///< G-buffer depth
        } g_buffer;
        std::unique_ptr<TextureCubemap> environment; ///< Filtered lighting environment
    };

    std::vector<SpecularProbe> probes_;
    std::unique_ptr<ComputeShader> relight_shader_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

/// \REFACTOR SHADERATTRIBITEINPUT INITS
/// \SEE_IMPORTANT_SHADER_CHANGE:
/// In light.frag.glsl make sure ambient diffuse has 1.0 - DFG LUT term applied
////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::SpecularLocal
/// \ingroup pipeline
///
/// Uses pre-integrated approximation for real-time specular relighting and
/// application. Described in DICE's Siggraph 2014 course "Moving Frostbite to
/// PBR": http://www.frostbite.com/2014/11/moving-frostbite-to-pbr/
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_SPECULARLOCAL_H_
