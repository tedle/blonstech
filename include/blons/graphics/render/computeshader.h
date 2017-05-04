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
    /// \param source_files List of source filenames and respective
    /// blons::ShaderPipelineStage types
    ////////////////////////////////////////////////////////////////////////////////
    ComputeShader(ShaderSourceList source_files);
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
    bool SetOutput(const char* field, TextureResource* value);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetOutput
    ///
    /// \param texture_index The slot to bind the texture to. Defaults to 0
    /// \param mip_level Mipmap level of the texture to bind. Defaults to 0
    ////////////////////////////////////////////////////////////////////////////////
    bool SetOutput(const char* field, TextureResource* value, unsigned int texture_index, unsigned int mip_level);
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
/// // Compile a ComputeShader
/// blons::ComputeShader compute({ { blons::ShaderPipelineStage::COMPUTE, "compute.glsl" } });
/// // Initialize ShaderData that will store the ComputeShader's output
/// std::vector<blons::Vector4> vector_array = { blons::Vector4(5.0f, 5.0f, 5.0f, 3.0f), blons::Vector4(0.0f, 0.0f, 0.0f, 4.0f) };
/// blons::ShaderData<blons::Vector4> array_data(vector_array.data(), vector_array.size());
/// // Bind the ShaderData as an input
/// compute.SetInput("array_data", array_data.data());
/// // Dispatch the ComputeShader
/// compute.Run(1, 1, 1);
///
/// // Retrieve new values from GPU memory
/// auto transformed_array = array_data.value();
/// for (int i = 0; i < array_data.length(); i++)
/// {
///     auto v = transformed_array[i];
///     blons::log::Debug("x:%.2f y:%.2f z:%.2f w:%.2f\n", v.x, v.y, v.z, v.w);
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_RENDER_COMPUTESHADER_H_
