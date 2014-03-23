#ifndef BLONSTECH_MESHIMPORTER_H_
#define BLONSTECH_MESHIMPORTER_H_

// Includes
#include <map>
#include <string>
#include <vector>
// Local Includes
#include "texture.h"

class MeshImporter
{
public:
    MeshImporter();
    ~MeshImporter();

    struct TextureInfo
    {
        std::string filename;
        Texture::Type type;
    };

    // Defaults invert_y to false
    bool Load(const char* filename);
    bool Load(const char* filename, bool invert_y);

    unsigned int vertex_count();
    unsigned int index_count();
    unsigned int uv_count();
    unsigned int normal_count();
    unsigned int face_count();
    // Anything returned by these getters will be freed on class destruction
    std::vector<Vertex>& vertices();
    std::vector<unsigned int>& indices();
    std::vector<TextureInfo>& textures();

private:
    unsigned int vertex_count_, index_count_, uv_count_, normal_count_, face_count_;
    std::vector<Vertex> vertices_;
    std::vector<unsigned int>indices_;
    std::vector<TextureInfo> textures_;
};
#endif