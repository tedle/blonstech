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
    Vector3 GetPos();
    Matrix GetWorldMatrix();

    void SetPos(float x, float y, float z);

private:
    bool InitMesh(WCHAR* filename);
    void FinishMesh();

    bool InitTexture(WCHAR* filename);
    void FinishTexture();

    Mesh* mesh_;
    Texture* texture_;
    Matrix world_matrix_;
    Vector3 pos_;
};

#endif