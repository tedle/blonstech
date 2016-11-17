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

#ifndef BLONSTECH_GRAPHICS_RENDER_COMPUTESHADER_H_
#define BLONSTECH_GRAPHICS_RENDER_COMPUTESHADER_H_

// Includes
#include <blons/graphics/render/render.h>
#include <blons/graphics/render/commonshader.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Class for creating general purpose compute shaders
////////////////////////////////////////////////////////////////////////////////
class ComputeShader : public CommonShader
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new compute shader using the provided source file
    ///
    /// \param source_filename Location of the compute shader source on disk
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    ComputeShader(std::string source_filename, RenderContext& context);
    ~ComputeShader();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Binds and runs the ComputeShader with the specified thread group
    /// dimensions. Further granularity of threads can usually be described at the
    /// shader level and in this case the total number of threads will be
    /// `(groups_x * shader_x) * (groups_y * shader_y) * (groups_z * shader_z)`
    ///
    /// \param groups_x Number of thread groups running on the X axis
    /// \param groups_y Number of thread groups running on the Y axis
    /// \param groups_z Number of thread groups running on the Z axis
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    bool Run(unsigned int groups_x, unsigned int groups_y, unsigned int groups_z,
             RenderContext& context);
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::ComputeShader
/// \ingroup graphics
///
/// The ComputeShader class manages compilation and resource management of
/// shader files used general purpose computing. Applies a custom preprocessor
/// before sending each shader to the API's compiler. For a list of preprocessor
/// directives see the CommonShader class documentation.
///
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_RENDER_COMPUTESHADER_H_
