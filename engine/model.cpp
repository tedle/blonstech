#include "model.h"

Model::Model()
{
    mesh_ = nullptr;
    texture_ = nullptr;
}

Model::~Model()
{
}

bool Model::Init(WCHAR* mesh_filename, WCHAR* texture_filename)
{
    if (!InitMesh(mesh_filename))
        return false;

    if (!InitTexture(texture_filename))
        return false;

    return true;
}

void Model::Finish()
{
    FinishMesh();
    FinishTexture();

    return;
}

void Model::Render()
{
    g_render->SetModelBuffer(mesh_->GetVertexBuffer(), mesh_->GetIndexBuffer());

    return;
}

int Model::GetIndexCount()
{
    return mesh_->GetIndexCount();
}

TextureResource* Model::GetTexture()
{
    return texture_->GetTexture();
}

bool Model::InitMesh(WCHAR* filename)
{
    mesh_ = new Mesh;

    if (!mesh_)
        return false;

    if (!mesh_->Init(filename))
        return false;

    return true;
}

void Model::FinishMesh()
{
    if (mesh_)
    {
        mesh_->Finish();
        delete mesh_;
        mesh_ = nullptr;
    }
    return;
}

bool Model::InitTexture(WCHAR* filename)
{
    texture_ = new Texture;

    if (!texture_)
        return false;

    if (!texture_->Init(filename))
        return false;

    return true;
}

void Model::FinishTexture()
{
    if (texture_)
    {
        texture_->Finish();
        delete texture_;
        texture_ = nullptr;
    }
    return;
}