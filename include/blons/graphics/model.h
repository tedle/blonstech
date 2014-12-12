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
class Model
{
public:
    Model(std::string mesh_filename, RenderContext& context);
    virtual ~Model();

    void Render(RenderContext& context);

    int index_count() const;
    const TextureResource* texture() const;
    Vector3 pos() const;
    Matrix world_matrix() const;

    void set_pos(units::world x, units::world y, units::world z);

protected:
    std::unique_ptr<class Mesh> mesh_;
    std::unique_ptr<Texture> diffuse_texture_;
    std::unique_ptr<Texture> normal_texture_;
    std::unique_ptr<Texture> light_texture_;
    Matrix world_matrix_;
    Vector3 pos_;
};
} // namespace blons

#endif // BLONSTECH_GRAPHICS_MODEL_H_
