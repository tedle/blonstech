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

#ifndef BLONSTECH_GRAPHICS_RENDER_DRAWBATCHER_H_
#define BLONSTECH_GRAPHICS_RENDER_DRAWBATCHER_H_

// Public Includes
#include <blons/graphics/mesh.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Utility for combining mesh data from various sources into a single
/// draw call
////////////////////////////////////////////////////////////////////////////////
class DrawBatcher
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a DrawBatcher with empty buffers.
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    DrawBatcher(RenderContext& context);
    ~DrawBatcher() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Appends mesh data to the draw batch for the current frame
    ///
    /// If the appending data causes the internal buffers to exceed their maximum
    /// capacity they are resized to be just large enough to contain the new data.
    /// Buffers will never be downsized however, and this is why it is best to
    /// recycle your batches!
    ///
    /// \param mesh_data Reference to the vertex and index data to store
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Append(const MeshData& mesh_data, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Pushes the DrawBatcher's buffers to the current rendering context,
    /// prepping it for a draw call
    ///
    /// Clears all stored mesh data, preventing it from being pushed again by
    /// subsequent calls (this does not actually free the memory, just invalidates
    /// the current data from being re-used).
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the number of valid, drawable indices from the current
    /// batch
    ///
    /// \return Number of indices to draw
    ////////////////////////////////////////////////////////////////////////////////
    int index_count() const;

private:
    std::unique_ptr<BufferResource> vertex_buffer_;
    std::unique_ptr<BufferResource> index_buffer_;

    unsigned int vertex_count_, index_count_;
    unsigned int vertex_idx_, index_idx_;
    unsigned int buffer_size_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::DrawBatcher
/// \ingroup graphics
///
/// This class uses a dynamically resized buffer to store mesh data and is
/// designed to recycle memory efficiently, so try keeping instances alive for
/// as long as possible. Note that blons::Graphics and blons::gui::Manager
/// already do efficient batching on their own, so only use this class when
/// implementing your own render pipeline.
/// 
/// ### Example:
/// \code
/// // Rendering a simple tile map as a single batch
/// static Sprite sprite("32x32 tile.png", context);
/// // DrawBatchers are most efficient when recycled!
/// static DrawBatcher batch;
///
/// // For each tile in a 10x10 grid, attach a unique sprite to the batch's mesh data
/// // Turns 100 draw calls into 1!
/// for (int x = 0; x < 10; x++)
/// {
///     for (int y = 0; y < 10; y++)
///     {
///         sprite->set_pos(x * 32, y * 32);
///         batch.Append(sprite->mesh());
///     }
/// }
///
/// // Set up various shader inputs...
/// shader->SetInput("world_matrix", MatrixIdentity(), context);
/// shader->SetInput("proj_matrix", orthographic_matrix, context);
/// shader->SetInput("diffuse", sprite->texture(), context);
///
/// // Push the batch's mesh data to the rendering pipeline
/// batch.Render(context);
///
/// // Do the draw call for the whole batch
/// shader->Render(batch.index_count(), context);
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_RENDER_DRAWBATCHER_H_