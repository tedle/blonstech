#ifndef BLONSTECH_GRAPHICS_RENDER_SHADER_H_
#define BLONSTECH_GRAPHICS_RENDER_SHADER_H_

// Includes
#include <blons/graphics/render/render.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Class for creating and interacting with a programmable rendering
/// pipeline
////////////////////////////////////////////////////////////////////////////////
class Shader
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new rendering pipeline using the provided shader source
    /// files and shader inputs
    ///
    /// \param vertex_filename Location of the vertex shader source on disk
    /// \param pixel_filename Location of the pixel (fragment) shader source on disk
    /// \param input %Vertex layout to be sent to the shader every frame
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    Shader(std::string vertex_filename, std::string pixel_filename, ShaderAttributeList inputs, RenderContext& context);
    ~Shader();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Issues a draw call for the specified number of indices
    ///
    /// \param index_count Number of indices to render
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(unsigned int index_count, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a shader's global variable to be that of the given value
    ///
    /// \param field Name of global variable to modify
    /// \param value Value to set global variable to
    /// \param context Handle to the current rendering context
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, int value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, Matrix value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, Vector3 value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, Vector4 value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, const TextureResource& value, RenderContext& context);

private:
    std::unique_ptr<ShaderResource> program_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Shader
/// \ingroup graphics
///
/// ### Example:
/// \code
/// // Creating a new shader
/// blons::ShaderAttributeList inputs;
/// inputs.push_back(blons::ShaderAttribute(0, "input_pos"));
/// inputs.push_back(blons::ShaderAttribute(1, "input_uv"));
/// inputs.push_back(blons::ShaderAttribute(2, "input_norm"));
/// auto shader = std::make_unique<blons::Shader>("vertex.glsl", "pixel.glsl", inputs, context));
///
/// // Make a quick model to render with the shader
/// auto model = std::make_unique<blons::Model>("model.bms", context);
///
/// // Rendering with a shader
/// while (true)
/// {
///     // Push the models vertices to the RenderContext
///     model->Render(context);
///
///     // Set various global variables of the shader
///     if (!shader->SetInput("world_matrix", model->world_matrix(), context) ||
///         !shader->SetInput("diffuse", model->texture(), context))
///     {
///         blons::log::Debug("Could not set shader inputs!\n");
///         continue;
///     }
///
///     // Do the draw call
///     shader->Render(model->index_count(), context);
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_RENDER_SHADER_H_