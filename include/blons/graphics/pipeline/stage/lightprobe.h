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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTPROBE_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTPROBE_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>

namespace blons
{
// Forward declarations
class DrawBatcher;
class Framebuffer;
class Shader;

namespace pipeline
{
namespace stage
{
// Forward declarations
class Geometry;
class Shadow;

class Lightprobe
{
public:
    enum Output
    {
        COEFFICIENTS,
        LIGHT_MAP_LOOKUP_POS,
        LIGHT_MAP_LOOKUP_NORMAL,
        DIRECT_LIGHT_MAP,
        INDIRECT_LIGHT_MAP,
        INDIRECT_LIGHT,
        PROBE_ALBEDO,
        PROBE_UV,
        PROBE
    };

public:
    Lightprobe(Perspective perspective, RenderContext& context);
    ~Lightprobe() {}

    bool Render(const Scene& scene, const Geometry& geometry, const Shadow& shadow, Perspective perspective,
                Matrix view_matrix, Matrix proj_matrix, Matrix light_vp_matrix, RenderContext& context);
    bool BuildLighting(const Scene& scene, RenderContext& context);

    const TextureResource* output(Output buffer) const;

private:
    // TODO: This should be done in the map file format, somehow
    // Or perhaps automatically thru a quad tree... or something...
    void GenerateProbes();

    // Should only be called once per map load
    bool BuildLightMapLookups(const Scene& scene, RenderContext& context);
    bool BuildProbeMaps(const Scene& scene, RenderContext& context);

    bool RenderLightmaps(const Scene& scene, const Shadow& shadow, Matrix light_vp_matrix, RenderContext& context);
    bool SumCoefficients(const Scene& scene, RenderContext& context);

    // Light map stuff
    std::unique_ptr<Shader> light_map_lookup_shader_;
    std::unique_ptr<Shader> direct_light_map_shader_;
    std::unique_ptr<Shader> indirect_light_map_shader_;
    std::unique_ptr<Framebuffer> light_map_lookup_buffer_;
    std::unique_ptr<Framebuffer> direct_light_map_accumulation_buffer_;
    std::unique_ptr<Framebuffer> indirect_light_map_accumulation_buffer_;
    Matrix light_map_ortho_matrix_;

    // Light probe stuff
    std::vector<Vector3> probes_;
    std::unique_ptr<DrawBatcher> probe_meshes_;
    std::unique_ptr<DrawBatcher> probe_quads_;
    std::unique_ptr<Camera> probe_view_;
    std::unique_ptr<Shader> probe_map_shader_;
    std::unique_ptr<Shader> probe_map_clear_shader_;
    std::unique_ptr<Shader> probe_shader_;
    std::unique_ptr<Shader> probe_coefficients_shader_;
    std::unique_ptr<Framebuffer> probe_map_buffer_;
    std::unique_ptr<Framebuffer> probe_buffer_;
    std::unique_ptr<Framebuffer> probe_coefficients_buffer_;
    Matrix probe_proj_matrix_, probe_ortho_matrix_;

    // Probe application
    std::unique_ptr<Shader> indirect_light_shader_;
    std::unique_ptr<Framebuffer> indirect_light_buffer_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::Lightprobe
/// \ingroup pipeline
/// 
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTPROBE_H_
