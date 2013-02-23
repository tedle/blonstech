#ifndef BLONSTECH_MODEL_H_
#define BLONSTECH_MODEL_H_

// Local Includes
#include "math.h"
#include "mesh.h"
#include "render.h"
#include "texture.h"

class Model
{
public:
    Model();
    ~Model();

    bool Init(WCHAR* mesh_filename, WCHAR* texture_filename);
    void Finish();
    void Render();

    int GetIndexCount();
    TextureResource* GetTexture();

private:
    bool InitMesh(WCHAR* filename);
    void FinishMesh();

    bool InitTexture(WCHAR* filename);
    void FinishTexture();

    Mesh* mesh_;
    Texture* texture_;
};

#endif