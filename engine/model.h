#ifndef BLONSTECH_MODEL_H_
#define BLONSTECH_MODEL_H_

// Includes
#include <memory>
// Local Includes
#include "math.h"
#include "mesh.h"
#include "meshimporter.h"
#include "render.h"
#include "texture.h"

class Model
{
public:
    Model();
    ~Model();

    bool Init(const char* mesh_filename);
    void Finish();
    void Render();

    int GetIndexCount();
    TextureResource* GetTexture();
    Vector3 GetPos();
    Matrix GetWorldMatrix();

    void SetPos(float x, float y, float z);

private:
    bool InitMesh(const char* filename);
    void FinishMesh();

    bool InitTexture(const char* filename, Texture::Type type);
    void FinishTexture();

    std::unique_ptr<Mesh> mesh_;
    std::unique_ptr<Texture> diffuse_texture_;
    std::unique_ptr<Texture> normal_texture_;
    std::unique_ptr<Texture> light_texture_;
    Matrix world_matrix_;
    Vector3 pos_;
};

#endif