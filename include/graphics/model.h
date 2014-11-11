#ifndef BLONSTECH_MODEL_H_
#define BLONSTECH_MODEL_H_

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
    Model(const char* mesh_filename, RenderContext& context);
    virtual ~Model();

    void Render(RenderContext& context);

    int index_count();
    TextureResource* texture();
    Vector3 pos();
    Matrix world_matrix();

    void set_pos(float x, float y, float z);

protected:
    void Init(const char* mesh_filename, RenderContext& context);

    std::unique_ptr<class Mesh> mesh_;
    std::unique_ptr<Texture> diffuse_texture_;
    std::unique_ptr<Texture> normal_texture_;
    std::unique_ptr<Texture> light_texture_;
    Matrix world_matrix_;
    Vector3 pos_;
};
} // namespace blons

#endif
