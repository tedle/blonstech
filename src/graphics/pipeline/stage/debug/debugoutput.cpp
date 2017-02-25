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

#include <blons/graphics/pipeline/stage/debug/debugoutput.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace debug
{
DebugOutput::DebugOutput(Perspective perspective)
{
    debug_output_buffer_.reset(new Framebuffer(perspective.width, perspective.height, { { TextureType::R8G8B8A8, TextureType::LINEAR, TextureType::CLAMP } }));
    probeview_.reset(new ProbeView());
    irradianceview_.reset(new IrradianceView());
}

bool DebugOutput::Render(const TextureResource* depth, const Scene& scene, const LightProbes& probes, const IrradianceVolume& irradiance, Matrix view_matrix, Matrix proj_matrix)
{
    debug_output_buffer_->Bind(Vector4(0, 0, 0, 0));
    if (!probeview_->Render(debug_output_buffer_.get(), depth, scene, probes, view_matrix, proj_matrix))
    {
        return false;
    }
    if (!irradianceview_->Render(debug_output_buffer_.get(), depth, scene, irradiance, view_matrix, proj_matrix))
    {
        return false;
    }
    return true;
}

const TextureResource* DebugOutput::output(Output buffer) const
{
    switch (buffer)
    {
    case Output::DEBUG:
        return debug_output_buffer_->textures()[0];
        break;
    default:
        return nullptr;
    }
}
} // namespace debug
} // namespace stage
} // namespace pipeline
} // namespace blons
