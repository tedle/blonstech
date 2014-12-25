#ifndef BLONSTECH_GRAPHICS_SPRITE_H_
#define BLONSTECH_GRAPHICS_SPRITE_H_

// Public Includes
#include <blons/graphics/mesh.h>
#include <blons/graphics/texture.h>
#include <blons/graphics/render/render.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains a quad and texture for quick and easy rendering of 2D
/// sprites
////////////////////////////////////////////////////////////////////////////////
class Sprite
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new sprite from an image file. Will throw on failure
    ///
    /// \param texture_filename Image file on disk
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    Sprite(std::string texture_filename, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new sprite from raw pixel data. Will throw on failure
    ///
    /// \param texture_data Pixel buffer and format info
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    Sprite(PixelData* texture_data, RenderContext& context);
    virtual ~Sprite() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Pushes quad to the graphics API preparing it for a draw call
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context);

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
    /// \brief Retrieves the texture used for rendering
    ///
    /// \return Texture resource
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource& texture() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the position of the sprite on screen in pixels
    ///
    /// \return %Sprite position
    ////////////////////////////////////////////////////////////////////////////////
    Vector2 pos() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the width and height of the sprite on screen in pixels
    ///
    /// \return %Sprite dimensions
    ////////////////////////////////////////////////////////////////////////////////
    Vector2 dimensions() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the raw mesh data of the quad used to render this sprite.
    /// Can be used instead of Sprite::Render to allow for drawcall batching with a
    /// blons::DrawBatcher
    ///
    /// \return %Sprite mesh data
    ////////////////////////////////////////////////////////////////////////////////
    const MeshData& mesh();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the position of the sprite on screen
    ///
    /// \param x Position of the sprite on the X axis in subpixel resolution
    /// \param y Position of the sprite on the Y axis in subpixel resolution
    ////////////////////////////////////////////////////////////////////////////////
    void set_pos(units::subpixel x, units::subpixel y);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the position and dimensions of the sprite on screen
    ///
    /// \param x Position of the sprite on the X axis in subpixel resolution
    /// \param y Position of the sprite on the Y axis in subpixel resolution
    /// \param w Width of the sprite in subpixel resolution
    /// \param h Height of the sprite in subpixel resolution
    ////////////////////////////////////////////////////////////////////////////////
    void set_pos(units::subpixel x, units::subpixel y, units::subpixel w, units::subpixel h);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the position of the sprite on screen
    ///
    /// \param x Position of the sprite on the X axis in pixels
    /// \param y Position of the sprite on the Y axis in pixels
    ////////////////////////////////////////////////////////////////////////////////
    void set_pos(units::pixel x, units::pixel y);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the position and dimensions of the sprite on screen
    ///
    /// \param x Position of the sprite on the X axis in pixels
    /// \param y Position of the sprite on the Y axis in pixels
    /// \param w Width of the sprite in pixels
    /// \param h Height of the sprite in pixels
    ////////////////////////////////////////////////////////////////////////////////
    void set_pos(units::pixel x, units::pixel y, units::pixel w, units::pixel h);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the position and dimensions of the sprite on screen
    ///
    /// \param pos Box containing the position and dimensions in subpixel resolution
    ////////////////////////////////////////////////////////////////////////////////
    void set_pos(const Box& pos);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the subregion of the texture for the sprite to render. Useful
    /// for spritesheets and such
    ///
    /// \param x Leftmost region of the texture to render in subpixel resolution
    /// \param y Topmost region of the texture to render in subpixel resolution
    /// \param w Width of the region to render in subpixel resolution
    /// \param h Height of the region to render in subpixel resolution
    ////////////////////////////////////////////////////////////////////////////////
    void set_subtexture(units::subpixel x, units::subpixel y, units::subpixel w, units::subpixel h);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the subregion of the texture for the sprite to render. Useful
    /// for spritesheets and such
    ///
    /// \param x Leftmost region of the texture to render in pixels
    /// \param y Topmost region of the texture to render in pixels
    /// \param w Width of the region to render in pixels
    /// \param h Height of the region to render in pixels
    ////////////////////////////////////////////////////////////////////////////////
    void set_subtexture(units::pixel x, units::pixel y, units::pixel w, units::pixel h);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the subregion of the texture for the sprite to render. Useful
    /// for spritesheets and such
    ///
    /// \param subtexture containing the region in subpixel resolution
    ////////////////////////////////////////////////////////////////////////////////
    void set_subtexture(const Box& subtexture);

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Called by the constructor, completes all generic initialization
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Init(RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Updates the internal mesh data to be based on the current position
    /// and texture region. Called by both Sprite::Render and Sprite::mesh
    ////////////////////////////////////////////////////////////////////////////////
    void BuildQuad();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reference to the vertices in the graphics API
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<BufferResource> vertex_buffer_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reference to the indices in the graphics API
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<BufferResource> index_buffer_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Mesh data of the quad used to render the sprite
    ////////////////////////////////////////////////////////////////////////////////
    MeshData mesh_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Texture of the sprite
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<Texture> texture_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Position and dimensions of the sprite
    ////////////////////////////////////////////////////////////////////////////////
    Box pos_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Region of the sprite's texture to render
    ////////////////////////////////////////////////////////////////////////////////
    Box tex_map_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Sprite
/// \ingroup graphics
///
/// ### Example:
/// \code
/// // Generally used with a graphics manager for automatic rendering
/// void SimpleRender()
/// {
///     // Creating the sprite
///     auto sprite = graphics->MakeSprite("sprite.png");
///
///     // Setting its position
///     sprite->set_pos(32, 32);
///
///     while (true)
///     {
///         // Render all sprites made through the blons::Graphics instance
///         graphics->Render();
///     }
///
///     // Sprite can be deleted at any time to remove it from the render loop
///     sprite.reset();
/// }
///
/// // For a manually setup rendering pipeline
/// void ManualRender()
/// {
///     // Creating the sprite
///     auto sprite = std::make_unique<blons::Sprite>("sprite.png");
///
///     // Setting its position
///     sprite->set_pos(32, 32);
///
///     while (true)
///     {
///         // Push the sprites's vertices
///         sprite->Render(context);
///
///         // Set the shader's inputs
///         shader->SetInput("diffuse", sprite->texture(), context);
///
///         // Make the draw call
///         shader->Render(sprite->index_count(), context);
///     }
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_SPRITE_H_