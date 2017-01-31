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

#include <blons/graphics/pipeline/stage/debug/debugoutput.h>

// Public Includes
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/render/shader.h>
#include <blons/graphics/render/shaderdata.h>
#include <blons/graphics/pipeline/stage/debug/probeview.h>

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
}

bool DebugOutput::Render(const TextureResource* depth, const LightProbes& probes, Matrix view_matrix, Matrix proj_matrix)
{
    debug_output_buffer_->Bind(Vector4(0, 0, 0, 0));
    return probeview_->Render(debug_output_buffer_.get(), depth, probes, view_matrix, proj_matrix);
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
