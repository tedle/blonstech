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
#include <blons/graphics/pipeline/scene.h>
#include <blons/system/client.h>

namespace blons
{
// Forward declarations
class DrawBatcher;
class Framebuffer;
class Shader;
class Sprite;

namespace pipeline
{
// Forward declarations
namespace stage { class Geometry; }
namespace stage { class Shadow; }
namespace stage { class Lightprobe; }
namespace stage { class Lighting; }

////////////////////////////////////////////////////////////////////////////////
/// \brief Provides an easy to use deferred rendering pipeline
////////////////////////////////////////////////////////////////////////////////
class Deferred
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which stage of the pipeline should be rendered to
    /// the screen
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        FINAL,                   ///< The final composited image
        ALBEDO,                  ///< Fullbright geometry
        NORMAL,                  ///< Worldspace normals of the geometry
        DEBUG,                   ///< Various debug information
        G_DEPTH,                 ///< Depth buffer of the geometry
        LIGHT_DEPTH,             ///< Depth buffer from the light's point of view
        DIRECT_LIGHT,            ///< Direct lighting pass
        PROBE_ALBEDO,            ///< Probe diffuse information
        PROBE_UV,                ///< Probe UV information
        LIGHT_MAP_LOOKUP_POS,    ///< Lightmap position lookup
        LIGHT_MAP_LOOKUP_NORMAL, ///< Lightmap worldspace normal lookup
        DIRECT_LIGHT_MAP,        ///< Direct lighting map
        INDIRECT_LIGHT_MAP,      ///< Bounce lighting map
        PROBE,                   ///< Shaded probe view
        PROBE_COEFFICIENTS,      ///< Spherical harmonics per probe
        INDIRECT_LIGHT,          ///< Bounce lighting pass
        NONE                     ///< Results in nullptr
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// Initializes a new deferred rendering pipeline using the supplied width and height
    /// as an output format. Will throw on failure
    ///
    /// \param screen Width and height to output rendering to
    /// \param fov Vertical field of view in radians
    /// \param screen_near Near screen clipping distance
    /// \param screen_far Far screen clipping distance
    ////////////////////////////////////////////////////////////////////////////////
    Deferred(Client::Info screen, float fov, float screen_near, float screen_far);
    ~Deferred();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders the supplied models and lights to the screen
    ///
    /// \param scene Struct containing all of the models and information needed to
    /// render a scene
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(const Scene& scene);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders the supplied models and lights to the supplied framebuffer
    ///
    /// \param scene Struct containing all of the models and information needed to
    /// render a scene
    /// \param output_buffer Borrowed pointer to a framebuffer for rendering
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(const Scene& scene, Framebuffer* output_buffer);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reloads graphics API. Used for applying new video settings. **Never**
    /// call this in the middle of rendering a frame (callbacks beware!).
    ///
    /// \param screen Width and height of the new pipeline output
    /// \param fov Vertical field of view in radians
    /// \param screen_near Near screen clipping distance
    /// \param screen_far Far screen clipping distance
    ////////////////////////////////////////////////////////////////////////////////
    void Reload(Client::Info screen, float fov, float screen_near, float screen_far);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to build global illumination for the scene. Should be called
    /// **once** after all static geometry has been loaded.
    ///
    /// \param scene Contains scene information for rendering
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool BuildLighting(const Scene& scene);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets which stage of the deferred rendering pipeline will be displayed
    /// to the screen after rendering
    ///
    /// \param output Rendering stage to display
    /// \param alt_output Rendering stage to display in bottom right corner
    ////////////////////////////////////////////////////////////////////////////////
    void set_output(Output output, Output alt_output);

private:
    bool Init();
    bool RenderComposite(const Scene& scene);

    Output output_, alt_output_;
    std::unique_ptr<Sprite> output_sprite_, alt_output_sprite_;

    Matrix proj_matrix_, ortho_matrix_;
    Perspective perspective_;

    // TODO: Document the pipeline
    std::unique_ptr<stage::Geometry> geometry_;
    std::unique_ptr<stage::Shadow> shadow_;
    std::unique_ptr<stage::Lightprobe> lightprobe_;
    std::unique_ptr<stage::Lighting> lighting_;

    std::unique_ptr<Shader> composite_shader_;
};
} // namespace pipeline
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::Deferred
/// \ingroup pipeline
/// 
/// ### Example:
/// \code
/// #include <blons/graphics/pipeline/pipeline.h>
///
/// // Using the deferred pipeline
/// auto pipeline = std::make_unique<blons::pipeline::Deferred>(screen, blons::kPi / 2.0f, screen_near, screen_far)
///
/// // Rendering loop
/// while (true)
/// {
///     auto context = blons::render::context();
///     blons::pipeline::Scene scene;
///     scene.lights = lights;
///     scene.models = models;
///     scene.sky_colour = sky_colour;
///     scene.view = camera;
///
///     context->BeginScene(Vector4(0, 0, 0, 1));
///
///     if (!pipeline->Render(scene))
///     {
///         // Uh oh!
///     }
///
///     // Can keep doing other rendering stuff here, UI, etc...
///
///     // Then swap buffers
///     context->EndScene();
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_DEFERRED_H_