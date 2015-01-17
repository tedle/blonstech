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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_DEFERRED_H_
#define BLONSTECH_GRAPHICS_PIPELINE_DEFERRED_H_

// Public Includes
#include <blons/graphics/camera.h>
#include <blons/graphics/light.h>
#include <blons/graphics/model.h>
#include <blons/system/client.h>

namespace blons
{
// Forward declarations
class DrawBatcher;
class Framebuffer;
class Shader;

namespace pipeline
{
struct Scene
{
    Camera view;                ///< Camera the scene is viewed from
    std::vector<Model*> models; ///< List of models to be rendered on screen
    std::vector<Light*> lights; ///< List of lights to be used in the scene
    Vector3 sky_colour;         ///< Colour of the sky
};
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for shadow map resolution
////////////////////////////////////////////////////////////////////////////////
const units::pixel kShadowMapResolution = 1024;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for light map resolution
////////////////////////////////////////////////////////////////////////////////
const units::pixel kLightMapResolution = 256;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for number of lighting bounces
////////////////////////////////////////////////////////////////////////////////
const int kLightBounces = 1;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for probemap resolution
////////////////////////////////////////////////////////////////////////////////
const int kProbeMapSize = 16;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for probemap area of effect
////////////////////////////////////////////////////////////////////////////////
const float kProbeDistance = 5.0f;

////////////////////////////////////////////////////////////////////////////////
/// \brief Class for an easy to use deferred rendering pipeline
////////////////////////////////////////////////////////////////////////////////
class Deferred
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// Initializes a new deferred rendering pipeline using the supplied width and height
    /// as an output format. Will throw on failure
    ///
    /// \param screen Width and height to output rendering to
    /// \param fov Vertical field of view in radians
    /// \param screen_near Near screen clipping distance
    /// \param screen_far Far screen clipping distance
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    Deferred(Client::Info screen, float fov, float screen_near, float screen_far, RenderContext& context);
    ~Deferred() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders the supplied models and lights to the screen
    ///
    /// \param scene Struct containing all of the models and information needed to
    /// render a scene
    /// \param context Handle to the current rendering context
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(const Scene& scene, RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reloads graphics API. Used for applying new video settings. **Never**
    /// call this in the middle of rendering a frame (callbacks beware!).
    ///
    /// \param screen Width and height of the new pipeline output
    /// \param fov Vertical field of view in radians
    /// \param screen_near Near screen clipping distance
    /// \param screen_far Far screen clipping distance
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Reload(Client::Info screen, float fov, float screen_near, float screen_far, RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to build global illumination for the scene. Should be called
    /// **once** after all static geometry has been loaded.
    ///
    /// \param context Handle to the current rendering context
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool BuildLighting(const Scene& scene, RenderContext& context);
private:
    bool Init(RenderContext& context);

    // Should only be called once per map load
    bool BuildLightMapLookups(const Scene& scene, RenderContext& context);
    bool BuildProbeMaps(const Scene& scene, RenderContext& context);

    bool RenderGeometry(const Scene& scene, Matrix view_matrix, RenderContext& context);
    bool RenderShadowMaps(const Scene& scene, Matrix view_matrix, Matrix light_vp_matrix, RenderContext& context);
    bool RenderLightMaps(const Scene& scene, Matrix light_vp_matrix, RenderContext& context);
    bool SumCoefficients(const Scene& scene, RenderContext& context); // RenderLightMaps helper
    bool RenderLighting(const Scene& scene, Matrix view_matrix, RenderContext& context);
    bool RenderComposite(const Scene& scene, RenderContext& context);

    Matrix proj_matrix_, ortho_matrix_;

    struct Perspective
    {
        units::pixel width, height;
        units::world screen_near, screen_far;
        float fov;
    } perspective_;

    // TODO: Document the pipeline
    std::unique_ptr<Shader> geo_shader_;
    std::unique_ptr<Shader> shadow_shader_;
    std::unique_ptr<Shader> blur_shader_;
    std::unique_ptr<Shader> direct_light_shader_;
    std::unique_ptr<Shader> indirect_light_shader_;
    std::unique_ptr<Shader> light_shader_;
    std::unique_ptr<Shader> composite_shader_;
    std::unique_ptr<Framebuffer> geometry_buffer_;
    std::unique_ptr<Framebuffer> shadow_buffer_;
    std::unique_ptr<Framebuffer> blur_buffer_;
    std::unique_ptr<Framebuffer> direct_light_buffer_;
    std::unique_ptr<Framebuffer> indirect_light_buffer_;
    std::unique_ptr<Framebuffer> light_buffer_;

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
};
} // namespace pipeline
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::DeferredPipeline
/// \ingroup graphics
/// 
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_DEFERRED_H_