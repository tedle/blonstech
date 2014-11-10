#ifndef BLONSTECH_DRAWBATCHER_H_
#define BLONSTECH_DRAWBATCHER_H_

// Includes
#include <vector>
// Local Includes
#include "mesh.h"

namespace blons
{
class DrawBatcher
{
public:
    DrawBatcher(RenderContext& context);
    ~DrawBatcher();

    void Append(MeshData* mesh_data);
    void Render(RenderContext& context);

    int index_count();

private:
    std::unique_ptr<BufferResource> vertex_buffer_;
    std::unique_ptr<BufferResource> index_buffer_;

    // We don't use mesh data struct here cus we need non-vector C99 super speeds :(
    std::unique_ptr<Vertex> vertices_;
    std::unique_ptr<unsigned int> indices_;
    unsigned int vertex_count_, index_count_;

    // We pretty much treat this as a resizable C array for perf reasons... ugly :(
    std::vector<MeshData> batch_;
    unsigned int batch_index_;
};
}
#endif