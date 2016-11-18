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
#include <blons/graphics/render/renderer.h>
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
    ////////////////////////////////////////////////////////////////////////////////
    ComputeShader(std::string source_filename);
    ~ComputeShader();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reloads the shader to be attached to the active rendering context
    /// Will throw on failure
    ////////////////////////////////////////////////////////////////////////////////
    void Reload();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Binds and runs the ComputeShader with the specified thread group
    /// dimensions. Further granularity of threads can usually be described at the
    /// shader level and in this case the total number of threads will be
    /// `(groups_x * shader_x) * (groups_y * shader_y) * (groups_z * shader_z)`
    ///
    /// \param groups_x Number of thread groups running on the X axis
    /// \param groups_y Number of thread groups running on the Y axis
    /// \param groups_z Number of thread groups running on the Z axis
    ////////////////////////////////////////////////////////////////////////////////
    bool Run(unsigned int groups_x, unsigned int groups_y, unsigned int groups_z);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a shader's global output variable to be that of the given value
    ///
    /// \param field Name of output variable to bind
    /// \param value Value to bind output variable to
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool SetOutput(const char* field, const TextureResource* value);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetOutput
    ///
    /// \param texture_index The slot to bind the texture to. Defaults to 0
    ////////////////////////////////////////////////////////////////////////////////
    bool SetOutput(const char* field, const TextureResource* value, unsigned int texture_index);

private:
    std::string source_filename_;
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
