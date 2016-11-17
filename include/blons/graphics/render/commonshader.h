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

#ifndef BLONSTECH_GRAPHICS_RENDER_COMMONSHADER_H_
#define BLONSTECH_GRAPHICS_RENDER_COMMONSHADER_H_

// Includes
#include <blons/graphics/render/render.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Houses code common to both pipeline Shader%s and ComputeShader%s.
/// Cannot be instantiated on its own
////////////////////////////////////////////////////////////////////////////////
class CommonShader
{
protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes an internal ShaderResource
    ////////////////////////////////////////////////////////////////////////////////
    CommonShader(RenderContext& context);

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a shader's global variable to be that of the given value
    ///
    /// \param field Name of global variable to modify
    /// \param value Value to set global variable to
    /// \param context Handle to the current rendering context
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, const float value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, const int value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, const Matrix value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, const Vector2 value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, const Vector3 value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, const Vector4 value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, const TextureResource* value, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc SetInput
    ///
    /// \param texture_index The slot to bind the texture to. Defaults to 0
    ////////////////////////////////////////////////////////////////////////////////
    bool SetInput(const char* field, const TextureResource* value, unsigned int texture_index, RenderContext& context);

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Loads source file into memory and applies preprocessor directives
    ///
    /// \param filename Source file on disk
    /// \return String containing processed source code. Will throw on failure
    ////////////////////////////////////////////////////////////////////////////////
    std::string ParseFile(std::string filename);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Handle to ShaderResource used by interal rendering API
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<ShaderResource> program_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::CommonShader
///
/// The CommonShader class is used to houses code shared by the Shader and
/// ComputeShader classes. Provides a handle to an internal ShaderResource,
/// functions for setting global shader variables, and a custom preprocessor
/// for added utility. Cannot be instantiated on its own.
///
/// ### Preprocessor Directives
/// Directive | Usage
/// --------- | -----
/// include   | <tt>\#include \<shaders/mesh.fx\></tt>
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_RENDER_COMMONSHADER_H_
