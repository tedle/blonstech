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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_BRDFLOOKUP_H_
#define BLONSTECH_GRAPHICS_PIPELINE_BRDFLOOKUP_H_

// Public Includes
#include <blons/graphics/texture.h>

namespace blons
{
namespace pipeline
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Generates a lookup texture for complex lighting terms used by a
/// bidirectional reflectance distribution function. Currently only supports
/// GGX diffuse and specular lighting
////////////////////////////////////////////////////////////////////////////////
class BRDFLookup
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which output should be retrieved from the BRDFLookup
    /// class
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        /// \VERIFY_THAT_THIS_IS_RIGHT
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Bidirectional reflectance distribution function lookup texture,
        /// used to pre-integrate complex lighting terms needed for ambient diffuse and
        /// specular lighting. Split into 4 colour channels, with light reconstruction
        /// functions as such:
        /// \code
        /// // light = incident lighting
        /// // f0 = Fresnel reflectance coefficient
        /// // albedo = surface colour
        /// Vector3 specular = light * (f0 * brdf.r + brdf.g);
        /// Vector3 diffuse = light * albedo * (brdf.b + albedo * brdf.a);
        /// \endcode
        ////////////////////////////////////////////////////////////////////////////////
        BRDF_LUT
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// Initializes a new BRDFLookup baker. Actual baking is deferred until
    /// a call to BakeLookupTexture() is made as the process is computationally
    /// expensive
    ////////////////////////////////////////////////////////////////////////////////
    BRDFLookup();
    ~BRDFLookup() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Bakes the bidirectional reflectance distribution function for
    /// diffuse and specular lighting into a lookup texture
    ////////////////////////////////////////////////////////////////////////////////
    void BakeLookupTexture();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the BRDFLookup class
    ///
    /// \param buffer BRDFLookup::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;

private:
    std::unique_ptr<Texture> brdf_lut_;
};
} // namespace pipeline
} // namespace blons

/// \NEEDS_CODE_SAMPLE
////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::BRDFLookup
/// \ingroup pipeline
/// 
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_BRDFLOOKUP_H_