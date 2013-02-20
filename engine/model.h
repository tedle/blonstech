#ifndef BLONSTECH_MODEL_H_
#define BLONSTECH_MODEL_H_

// Local Includes
#include "math.h"
#include "render.h"
#include "texture.h"

class Model
{
public:
    Model();
    ~Model();

    bool Init(WCHAR*);
    void Finish();
    void Render();

    int  GetIndexCount();

    TextureResource* GetTexture();

private:
    bool InitBuffers();
    void FinishBuffers();

    bool LoadTexture(WCHAR*);
    void ReleaseTexture();

    BufferResource *vertex_buffer_, *index_buffer_;
    int vertex_count_, index_count_;
    Texture* texture_;
};

#endif