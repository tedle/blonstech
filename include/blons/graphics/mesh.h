#ifndef BLONSTECH_GRAPHICS_MESH_H_
#define BLONSTECH_GRAPHICS_MESH_H_

// Includes
#include <memory>
// Public Includes
#include <blons/graphics/render/render.h>

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
    Mesh(const class MeshImporter& mesh_data, RenderContext& context);
    ~Mesh();

    BufferResource* vertex_buffer() const;
    BufferResource* index_buffer() const;
    unsigned int vertex_count() const;
    unsigned int index_count() const;

private:
    std::unique_ptr<BufferResource> vertex_buffer_, index_buffer_;
    MeshData mesh_data_;
};
} // namespace blons

#endif // BLONSTECH_GRAPHICS_MESH_H_
