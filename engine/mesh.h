#ifndef BLONSTECH_MESH_H_
#define BLONSTECH_MESH_H_

// Includes
#include <map>
#include <memory>
#include <stdio.h>
#include <vector>
// Local Includes
#include "render.h"

// Holds mesh info in memory while being reorganized to renderable format
struct RawMesh
{
    unsigned int vertex_count, uv_count, normal_count, face_count;
    std::vector<Vector3> vertices, normals;
    std::vector<Vector2> uvs;
    std::vector<unsigned int> faces;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    bool Init(const char* filename, bool invert_y);
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