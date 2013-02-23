#ifndef BLONSTECH_MESH_H_
#define BLONSTECH_MESH_H_

// Includes
#include <stdio.h>

#include "render.h"

// Holds mesh info in memory while being reorganized to renderable format
struct RawMesh
{
    unsigned int vertex_count, uv_count, normal_count, face_count;
    Vector3* vertices, *normals;
    Vector2* uvs;
    unsigned int* faces;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    bool Init(WCHAR* filename);
    void Finish();

    BufferResource* GetVertexBuffer();
    BufferResource* GetIndexBuffer();
    int GetVertexCount();
    int GetIndexCount();

private:
    BufferResource *vertex_buffer_, *index_buffer_;
    int vertex_count_, index_count_;
};

#endif