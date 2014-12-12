#ifndef BLONSTECH_GRAPHICS_MESHIMPORTER_H_
#define BLONSTECH_GRAPHICS_MESHIMPORTER_H_

// Includes
#include <string>
#include <vector>
// Public Includes
#include <blons/graphics/mesh.h>
#include <blons/graphics/texture.h>

namespace blons
{
class MeshImporter
{
public:
    // Defaults invert_y to false
    MeshImporter(std::string filename);
    MeshImporter(std::string filename, bool invert_y);
    ~MeshImporter();

    struct TextureInfo
    {
        std::string filename;
        Texture::Type type;
    };

    unsigned int vertex_count() const;
    unsigned int index_count() const;
    unsigned int uv_count() const;
    unsigned int normal_count() const;
    unsigned int face_count() const;
    // Anything returned by these getters will be freed on class destruction
    const MeshData* mesh_data() const;
    const std::vector<TextureInfo>* textures() const;

private:
    bool Load(std::string filename, bool invert_y);

    unsigned int uv_count_, normal_count_, face_count_;
    MeshData mesh_data_;
    std::vector<TextureInfo> textures_;
};
} // namespace blons

#endif // BLONSTECH_GRAPHICS_MESHIMPORTER_H_