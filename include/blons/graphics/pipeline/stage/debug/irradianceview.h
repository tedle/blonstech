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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_IRRADIANCEVIEW_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_IRRADIANCEVIEW_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/pipeline/stage/irradiancevolume.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/render/shader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace debug
{
class IrradianceView
{
public:
    IrradianceView();
    ~IrradianceView() {}

    bool Render(Framebuffer* target, const TextureResource* depth, const IrradianceVolume& irradiance, Matrix view_matrix, Matrix proj_matrix);

private:
    // Full init is deferred until first Render() because it's optional and adds significant startup time
    void InitMeshBuffers(const IrradianceVolume& irradiance);

    std::unique_ptr<DrawBatcher> grid_mesh_;
    std::unique_ptr<DrawBatcher> voxel_meshes_;
    std::unique_ptr<Shader> grid_shader_;
    std::unique_ptr<Shader> volume_shader_;
};
} // namespace debug
} // namespace stage
} // namespace pipeline
} // namespace blons

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_IRRADIANCEVIEW_H_
