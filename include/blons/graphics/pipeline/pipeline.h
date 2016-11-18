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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_PIPELINE_H_
#define BLONSTECH_GRAPHICS_PIPELINE_PIPELINE_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/pipeline/stage/geometry.h>
#include <blons/graphics/pipeline/stage/shadow.h>
#include <blons/graphics/pipeline/stage/lightprobe.h>
#include <blons/graphics/pipeline/stage/lighting.h>
#include <blons/graphics/pipeline/deferred.h>

////////////////////////////////////////////////////////////////////////////////
/// \defgroup pipeline Pipeline
/// \ingroup graphics
/// \brief 3D rendering pipelines
/// 
/// Provides a modular approach to building a rendering pipeline from predefined
/// stages, as well as pre-built pipelines for easy use.
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
///
///
///
/// // Building your own pipeline
/// // (This is what the deferred renderer does internally)
/// blons::pipeline::Perspective perspective;
/// perspective.width = 1280; // Screen width
/// perspective.height = 720; // Screen height
/// perspective.screen_near = // Near clipping range
/// perspective.screen_far = // Far clipping range
/// perspective.fov = fov; // Vertical fov in radians
///
/// // Projection matrix (3D space->2D screen)
/// float screen_aspect = static_cast<float>(perspective.width) / static_cast<float>(perspective.height);
/// Matrix proj_matrix = MatrixPerspective(perspective.fov, screen_aspect, perspective.screen_near, perspective.screen_far);
///
/// // Ortho projection matrix (for 2d stuff, shadow maps, etc)
/// Matrix ortho_matrix = MatrixOrthographic(0, units::pixel_to_subpixel(perspective.width),
///                                          units::pixel_to_subpixel(perspective.height), 0,
///                                          perspective.screen_near, perspective.screen_far);
///
/// // Pipeline setup
/// auto geometry = std::make_unique<blons::pipeline::stage::Geometry>(perspective);
/// auto shadow = std::make_unique<blons::pipeline::stage::Shadow>(perspective);
/// auto lightprobe = std::make_unique<blons::pipeline::stage::Lightprobe>(perspective);
/// auto lighting = std::make_unique<blons::pipeline::stage::Lighting>(perspective);
///
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
///     // Calculates view_matrix from scratch, so we cache it
///     Matrix view_matrix = scene.view.view_matrix();
///     Matrix light_vp_matrix = sun->ViewFrustum(view_matrix * proj_matrix, perspective.screen_far);
///
///     // Render all of the geometry and accompanying info (normal, depth, etc)
///     if (!geometry->Render(scene, view_matrix, proj_matrix))
///     {
///         // That's not good
///     }
///
///     // Render all of the geometry and get their depth from the light's point of view
///     // Then render a shadow map from the depth information
///     if (!shadow->Render(scene, *geometry, view_matrix, proj_matrix, light_vp_matrix, ortho_matrix))
///     {
///         // Better panic
///     }
///
///     // Builds a direct light map and then bounce lighting
///     if (!lightprobe->Render(scene, *geometry, *shadow, perspective,
///                              view_matrix, proj_matrix, light_vp_matrix))
///     {
///         // Oughta get out of here
///     }
///
///     if (!lighting->Render(scene, *geometry, *shadow, *lightprobe, view_matrix, proj_matrix, ortho_matrix))
///     {
///         // Whoops
///     }
///
///     // Bind the back buffer
///     context->BindFramebuffer(nullptr);
///
///     // Grab the final scene in this pipeline
///     const blons::TextureResource* output = lighting->output(blons::pipeline::stage::Lighting::LIGHT);
///
///     // Render it however you like... sprites are pretty easy
///
///     // Then push it to the screen
///     context->EndScene();
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \namespace blons::pipeline
/// \brief Encapsulates classes related to the rendering pipeline
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_PIPELINE_H_
