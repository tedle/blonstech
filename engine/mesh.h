#ifndef BLONSTECH_MESH_H_
#define BLONSTECH_MESH_H_

// Includes
#include <map>
#include <memory>
#include <stdio.h>
#include <vector>
// Local Includes
#include "meshimporter.h"
#include "render.h"


class Mesh
{
public:
    Mesh(MeshImporter* mesh_data, RenderContext& context);
    ~Mesh();

    BufferResource* vertex_buffer();
    BufferResource* index_buffer();
    int vertex_count();
    int index_count();

private:
    std::unique_ptr<BufferResource> vertex_buffer_, index_buffer_;
    unsigned int vertex_count_, index_count_;
};

#endif