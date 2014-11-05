#ifndef BLONSTECH_MODEL_H_
#define BLONSTECH_MODEL_H_

// Includes
#include <memory>
// Local Includes
#include "render.h"
#include "texture.h"

namespace blons
{
class Model
{
public:
    Model();
    ~Model();

    bool Load(const char* mesh_filename, RenderContext& context);
    void Finish(RenderContext& context);
    void Render(RenderContext& context);

    int index_count();
    TextureResource* texture();
    Vector3 pos();
    Matrix world_matrix();

    void set_pos(float x, float y, float z);

private:
    bool LoadMesh(const char* filename);
    void FinishMesh(RenderContext& context);

    bool LoadTexture(const char* filename, Texture::Type type);
    void FinishTexture(RenderContext& context);

    std::unique_ptr<class Mesh> mesh_;
    std::unique_ptr<Texture> diffuse_texture_;
    std::unique_ptr<Texture> normal_texture_;
    std::unique_ptr<Texture> light_texture_;
    Matrix world_matrix_;
    Vector3 pos_;
};
} // namespace blons

#endif