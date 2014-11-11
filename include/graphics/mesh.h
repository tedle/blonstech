#ifndef BLONSTECH_MESH_H_
#define BLONSTECH_MESH_H_

// Includes
#include <memory>
// Local Includes
#include "render/render.h"

namespace blons
{
struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

class Mesh
{
public:
    Mesh(class MeshImporter* mesh_data, RenderContext& context);
    ~Mesh();

    BufferResource* vertex_buffer();
    BufferResource* index_buffer();
    int vertex_count();
    int index_count();

private:
    std::unique_ptr<BufferResource> vertex_buffer_, index_buffer_;
    MeshData mesh_data_;
};
} // namespace blons

#endif