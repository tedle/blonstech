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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_SCENE_H_
#define BLONSTECH_GRAPHICS_PIPELINE_SCENE_H_

// Public Includes
#include <blons/graphics/camera.h>
#include <blons/graphics/light.h>
#include <blons/graphics/model.h>

namespace blons
{
namespace pipeline
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Data object containing a scene to be rendered by pipelines
////////////////////////////////////////////////////////////////////////////////
struct Scene
{
    Camera view;                    ///< Camera the scene is viewed from
    std::vector<Model*> models;     ///< List of models to be rendered on screen
    std::vector<Light*> lights;     ///< List of lights to be used in the scene
    SHColourCoeffs3 sky_box;        ///< Sky box mapped to spherical harmonics
    units::luminance sky_luminance; ///< Luminance emitted by sky
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Data object containing information about how a scene is viewed
////////////////////////////////////////////////////////////////////////////////
struct Perspective
{
    units::pixel width;       ///< Screen width in pixels
    units::pixel height;      ///< Screen height in pixels
    units::world screen_near; ///< Near clipping range in world units
    units::world screen_far;  ///< Far clipping range in world units
    float fov;                ///< Vertical field of view in radians
};

////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for shadow map resolution
////////////////////////////////////////////////////////////////////////////////
const units::pixel kShadowMapResolution = 1024;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for specular probe resolution
////////////////////////////////////////////////////////////////////////////////
const int kSpecularProbeMapSize = 512;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for specular probe mip levels
////////////////////////////////////////////////////////////////////////////////
const int kSpecularProbeMipLevels = 5;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for probemap resolution
////////////////////////////////////////////////////////////////////////////////
const int kProbeMapSize = 16;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** Length of a LightSector::Surfel edge in world units
////////////////////////////////////////////////////////////////////////////////
const units::world kSurfelSize = 0.8f;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** Number of surfels per brick per dimension
////////////////////////////////////////////////////////////////////////////////
const int kSurfelsPerBrick = 4;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for irradiance volume resolution
////////////////////////////////////////////////////////////////////////////////
const int kIrradianceVolumeWidth = 32;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for irradiance volume resolution
////////////////////////////////////////////////////////////////////////////////
const int kIrradianceVolumeHeight = 16;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for irradiance volume resolution
////////////////////////////////////////////////////////////////////////////////
const int kIrradianceVolumeDepth = 16;
} // namespace pipeline
} // namespace blons

#endif // BLONSTECH_GRAPHICS_PIPELINE_SCENE_H_
