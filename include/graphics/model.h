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
    ~Model();

    int index_count();
    TextureResource* texture();
    Vector3 pos();
    Matrix world_matrix();

    void set_pos(float x, float y, float z);

private:
    // So we can return models to users and let them modify without
    // getting confused why "Render" does nothing for them
    friend class Graphics;
    // vvv This is preferable, but intellisense bug shows this as an error
    //friend bool Graphics::Render();
    void Render(RenderContext& context);
    // Also for this... which is really pretty dirty, but convenient for the user... i think
    // Lets graphics class keep a list of living sprites & models
    std::function<void(Model*)> deleter_;

    std::unique_ptr<class Mesh> mesh_;
    std::unique_ptr<Texture> diffuse_texture_;
    std::unique_ptr<Texture> normal_texture_;
    std::unique_ptr<Texture> light_texture_;
    Matrix world_matrix_;
    Vector3 pos_;
};
} // namespace blons

#endif