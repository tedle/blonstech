#ifndef BLONSTECH_MESHIMPORTER_H_
#define BLONSTECH_MESHIMPORTER_H_

// Includes
#include <string>
#include <vector>
// Local Includes
#include "mesh.h"
#include "texture.h"

namespace blons
{
class MeshImporter
{
public:
    // Defaults invert_y to false
    MeshImporter(const char* filename);
    MeshImporter(const char* filename, bool invert_y);
    ~MeshImporter();

    struct TextureInfo
    {
        std::string filename;
        Texture::Type type;
    };

    unsigned int vertex_count();
    unsigned int index_count();
    unsigned int uv_count();
    unsigned int normal_count();
    unsigned int face_count();
    // Anything returned by these getters will be freed on class destruction
    MeshData* mesh_data();
    std::vector<TextureInfo>* textures();

private:
    bool Load(const char* filename, bool invert_y);

    unsigned int uv_count_, normal_count_, face_count_;
    MeshData mesh_data_;
    std::vector<TextureInfo> textures_;
};
} // namespace blons

#endif