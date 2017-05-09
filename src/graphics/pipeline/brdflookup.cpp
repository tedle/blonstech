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

#include <blons/graphics/pipeline/brdflookup.h>

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/render/computeshader.h>

namespace blons
{
namespace pipeline
{
BRDFLookup::BRDFLookup() {}

void BRDFLookup::BakeLookupTexture()
{
    log::Debug("Baking BRDF lookup texture... ");
    PixelData lut;
    lut.type = TextureType(TextureType::R16G16B16A16_UNORM, TextureType::RAW, TextureType::LINEAR, TextureType::CLAMP);
    lut.width = kBRDFLookupSize;
    lut.height = kBRDFLookupSize;
    brdf_lut_.reset(new Texture(lut));
    Timer brdf_bake_stats;
    ComputeShader bake_shader({ { COMPUTE, "shaders/brdf-lut-bake.comp.glsl" } });
    bake_shader.SetOutput("brdf_lut", brdf_lut_->mutable_texture().get());
    bake_shader.Run(kBRDFLookupSize, kBRDFLookupSize, 1);
    log::Debug("[%ims]\n", brdf_bake_stats.ms());
}

const TextureResource* BRDFLookup::output(Output buffer) const
{
    switch (buffer)
    {
    case BRDF_LUT:
        if (brdf_lut_ == nullptr)
        {
            throw "Attempt to access BRDF LUT before baking";
        }
        return brdf_lut_->texture();
    default:
        throw "Unknown output requested from BRDFLookup";
    }
}
} // namespace pipeline
} // namespace blons