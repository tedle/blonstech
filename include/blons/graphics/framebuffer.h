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

#ifndef BLONSTECH_GRAPHICS_FRAMEBUFFER_H_
#define BLONSTECH_GRAPHICS_FRAMEBUFFER_H_

#include <blons/graphics/render/render.h>
#include <blons/graphics/mesh.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// Utility for managing framebuffers in the graphics API
////////////////////////////////////////////////////////////////////////////////
class Framebuffer
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new framebuffer of specified width and height with an
    /// expected output to the supplied number of textures. Will throw on failure
    ///
    /// \param width Width of the render target in pixels
    /// \param height Height of the render target in pixels
    /// \param texture_outputs Number of render targets
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    Framebuffer(units::pixel width, units::pixel height, unsigned int texture_outputs, RenderContext& context);
    ~Framebuffer() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Convenience function for drawing render targets to the screen in a 2D
    /// fashion.
    ///
    /// For more 3D uses you currently have to supply your own mesh data
    /// and retrieve the render targets through the textures() function.
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Binds the framebuffer to the supplied rendering context causing all
    /// subsequent draw calls to be drawn to this buffer
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Bind(RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Unbinds the framebuffer causing all subsequent draw calls to be drawn
    /// to the default back buffer
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Unbind(RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Number of vertices in the quad used for rendering. Should always be
    /// 4, really
    ///
    /// \return Number of vertices
    ////////////////////////////////////////////////////////////////////////////////
    unsigned int vertex_count() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Number of indices in the quad used for rendering. Should always be
    /// 6, really
    ///
    /// \return Number of indices
    ////////////////////////////////////////////////////////////////////////////////
    unsigned int index_count() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves an array of all the render targets of the framebuffer as
    /// TextureResource%s
    ///
    /// \return List of render targets
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<const TextureResource*> textures();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the depth texture of the framebuffer as a TextureResource.
    /// May return nullptr if no depth buffer is used for rendering
    ///
    /// \return Depth texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* depth();

private:
    std::unique_ptr<FramebufferResource> fbo_;

    // These are cleaned up by the FramebufferResource itself
    std::vector<const TextureResource*> textures_;
    const TextureResource* depth_texture_;

    // Used for rendering the targets to screen conveniently
    std::unique_ptr<BufferResource> vertex_buffer_;
    std::unique_ptr<BufferResource> index_buffer_;
    MeshData render_quad_;
};
} // namespace blons

#endif // BLONSTECH_GRAPHICS_FRAMEBUFFER_H_