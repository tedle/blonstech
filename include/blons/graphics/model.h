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

#ifndef BLONSTECH_GRAPHICS_MODEL_H_
#define BLONSTECH_GRAPHICS_MODEL_H_

// Includes
#include <functional>
#include <memory>
// Public Includes
#include <blons/graphics/texture.h>
#include <blons/graphics/render/render.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains a mesh and textures for quick and easy rendering of 3D
/// models
////////////////////////////////////////////////////////////////////////////////
class Model
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a model using the supplied blonsmesh (.bms) file, assumes
    /// texture files to be located in "../tex/" (**temporary**)
    ///
    /// \param mesh_filename Location of the mesh on disk to load
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    Model(std::string mesh_filename, RenderContext& context);
    virtual ~Model() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Pushes mesh data to the graphics API as well as updates the world
    /// matrix
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reloads the model to be active in the supplied rendering context
    ///
    /// \param context Handle to the current rendering context
    /// \return True if reinitialized successfully
    ////////////////////////////////////////////////////////////////////////////////
    bool Reload(RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the number of indices contained in the mesh
    ///
    /// \return Number of indices
    ////////////////////////////////////////////////////////////////////////////////
    int index_count() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the raw vertex and index data of the mesh
    ///
    /// \return Mesh data
    ////////////////////////////////////////////////////////////////////////////////
    const MeshData& mesh() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the albedo texture of the model
    ///
    /// \return Albedo texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* albedo() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the normal texture of the model
    ///
    /// \return Normal texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* normal() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the lightmap texture of the model
    ///
    /// \return Lightmap texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* lightmap() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the position of the model
    ///
    /// \return %Model position
    ////////////////////////////////////////////////////////////////////////////////
    Vector3 pos() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a world matrix indicating the position of the model.
    /// Updated when Model::Render is called
    ///
    /// \return %Model world matrix
    ////////////////////////////////////////////////////////////////////////////////
    Matrix world_matrix() const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Updates the model's position in the world
    ///
    /// \param x Position to move model to on the X axis
    /// \param y Position to move model to on the Y axis
    /// \param z Position to move model to on the Z axis
    ////////////////////////////////////////////////////////////////////////////////
    void set_pos(units::world x, units::world y, units::world z);

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Contains vertices and indices of the model
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<class Mesh> mesh_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Albedo texture of the model
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<Texture> albedo_texture_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Normal texture of the model
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<Texture> normal_texture_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Lightmap of the model
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<Texture> light_texture_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief World matrix indicating position of model, updated when Model::Render
    /// is called
    ////////////////////////////////////////////////////////////////////////////////
    Matrix world_matrix_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Position of the model
    ////////////////////////////////////////////////////////////////////////////////
    Vector3 pos_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Model
/// \ingroup graphics
///
/// ### Example:
/// \code
/// // Generally used with a graphics manager for automatic rendering
/// void SimpleRender()
/// {
///     // Creating the model
///     auto model = graphics->MakeModel("model.bms");
///
///     // Setting its position
///     model->set_pos(10.0f, 10.0f, 10.0f);
///
///     while (true)
///     {
///         // Render all models made through the blons::Graphics instance
///         graphics->Render();
///     }
///
///     // Model can be deleted at any time to remove it from the render loop
///     model.reset();
/// }
///
/// // For a manually setup rendering pipeline
/// void ManualRender()
/// {
///     // Creating the model
///     auto model = std::make_unique<blons::Model>("model.bms");
///
///     // Setting its position
///     model->set_pos(10.0f, 10.0f, 10.0f);
///
///     while (true)
///     {
///         // Push the model's vertices and update world matrix
///         model->Render(context);
///
///         // Set the shader's inputs
///         shader->SetInput("world_matrix", model->world_matrix(), context);
///         shader->SetInput("albedo", model->albedo(), context);
///
///         // Make the draw call
///         shader->Render(model->index_count(), context);
///     }
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_MODEL_H_
