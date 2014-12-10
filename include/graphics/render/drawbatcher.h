#ifndef BLONSTECH_GRAPHICS_RENDER_DRAWBATCHER_H_
#define BLONSTECH_GRAPHICS_RENDER_DRAWBATCHER_H_

// Local Includes
#include <graphics/mesh.h>

namespace blons
{
// Yo this class is pretty much C99-like to make it much faster
// Also this class is intended to be RE-USED!!! Save yourself from needless re-allocations!
class DrawBatcher
{
public:
    DrawBatcher(RenderContext& context);
    ~DrawBatcher() {}

    void Append(const MeshData& mesh_data);
    void Render(RenderContext& context);

    int index_count() const;

private:
    std::unique_ptr<BufferResource> vertex_buffer_;
    std::unique_ptr<BufferResource> index_buffer_;

    std::unique_ptr<Vertex> vertices_;
    std::unique_ptr<unsigned int> indices_;
    unsigned int vertex_count_, index_count_;
    unsigned int vertex_idx_, index_idx_;
    std::size_t array_size_;
};
}
#endif