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
    Mesh();
    ~Mesh();

    bool Init(MeshImporter* mesh_data);
    void Finish();

    BufferResource* GetVertexBuffer();
    BufferResource* GetIndexBuffer();
    int GetVertexCount();
    int GetIndexCount();

private:
    std::unique_ptr<BufferResource> vertex_buffer_, index_buffer_;
    unsigned int vertex_count_, index_count_;
};

#endif