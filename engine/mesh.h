#ifndef BLONSTECH_MESH_H_
#define BLONSTECH_MESH_H_

// Includes
#include <memory>
// Local Includes
#include "render.h"


class Mesh
{
public:
    Mesh();
    ~Mesh();

    bool Load(class MeshImporter* mesh_data, RenderContext& context);
    void Finish(RenderContext& context);

    BufferResource* vertex_buffer();
    BufferResource* index_buffer();
    int vertex_count();
    int index_count();

private:
    std::unique_ptr<BufferResource> vertex_buffer_, index_buffer_;
    unsigned int vertex_count_, index_count_;
};

#endif