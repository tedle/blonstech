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

#ifndef BLONSTECH_GRAPHICS_RENDER_SHADER_H_
#define BLONSTECH_GRAPHICS_RENDER_SHADER_H_

// Includes
#include <blons/graphics/render/renderer.h>
#include <blons/graphics/render/commonshader.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Class for creating and interacting with a programmable rendering
/// pipeline
////////////////////////////////////////////////////////////////////////////////
class Shader : public CommonShader
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new rendering pipeline using the provided shader source
    /// files and shader inputs
    ///
    /// \param vertex_filename Location of the vertex shader source on disk
    /// \param pixel_filename Location of the pixel (fragment) shader source on disk
    /// \param inputs %Vertex layout to be sent to the shader every frame
    ////////////////////////////////////////////////////////////////////////////////
    Shader(std::string vertex_filename, std::string pixel_filename, ShaderAttributeList inputs);
    ~Shader();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reloads the shader to be attached to the active rendering context
    /// Will throw on failure
    ////////////////////////////////////////////////////////////////////////////////
    void Reload();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Issues a draw call for the specified number of indices
    ///
    /// \param index_count Number of indices to render
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(unsigned int index_count);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Issues a draw call for the specified number of indices and instances
    ///
    /// \param index_count Number of indices to render
    /// \param instance_count Number of instances to render
    ////////////////////////////////////////////////////////////////////////////////
    bool RenderInstanced(unsigned int index_count, unsigned int instance_count);

private:
    std::string vertex_filename_;
    std::string pixel_filename_;
    ShaderAttributeList inputs_;
};
} // namespace blons

// TODO: When preprocessor is moved to standalone class from CommonShader, update docs
// to reference new class both in Shader and ComputeShader
////////////////////////////////////////////////////////////////////////////////
/// \class blons::Shader
/// \ingroup graphics
///
/// The Shader class manages compilation and resource management of shader files
/// used in rendering pipelines. Applies a custom preprocessor before sending
/// each shader to the API's compiler. For a list of preprocessor directives see
/// the CommonShader class documentation.
///
/// ### Example:
/// \code
/// // Creating a new shader
/// blons::ShaderAttributeList inputs;
/// inputs.push_back(blons::ShaderAttribute(0, "input_pos"));
/// inputs.push_back(blons::ShaderAttribute(1, "input_uv"));
/// inputs.push_back(blons::ShaderAttribute(2, "input_norm"));
/// auto shader = std::make_unique<blons::Shader>("vertex.glsl", "pixel.glsl", inputs));
///
/// // Make a quick model to render with the shader
/// auto model = std::make_unique<blons::Model>("model.bms");
///
/// // Rendering with a shader
/// while (true)
/// {
///     // Push the models vertices to the GPU
///     model->Render();
///
///     // Set various global variables of the shader
///     if (!shader->SetInput("world_matrix", model->world_matrix()) ||
///         !shader->SetInput("albedo", model->texture()))
///     {
///         blons::log::Debug("Could not set shader inputs!\n");
///         continue;
///     }
///
///     // Do the draw call
///     shader->Render(model->index_count());
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_RENDER_SHADER_H_