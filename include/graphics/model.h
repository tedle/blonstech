#ifndef BLONSTECH_GRAPHICS_MODEL_H_
#define BLONSTECH_GRAPHICS_MODEL_H_

// Includes
#include <functional>
#include <memory>
// Local Includes
#include "graphics/texture.h"
#include "render/render.h"

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

    void set_pos(float x, float y, float z);

protected:
    void Init(std::string mesh_filename, RenderContext& context);

    std::unique_ptr<class Mesh> mesh_;
    std::unique_ptr<Texture> diffuse_texture_;
    std::unique_ptr<Texture> normal_texture_;
    std::unique_ptr<Texture> light_texture_;
    Matrix world_matrix_;
    Vector3 pos_;
};
} // namespace blons

#endif
