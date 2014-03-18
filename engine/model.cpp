#include "model.h"

Model::Model()
{
    mesh_ = nullptr;
    texture_ = nullptr;
    pos_ = Vector3(0.0f, 0.0f, 0.0f);
    world_matrix_ = MatrixIdentity();
}

Model::~Model()
{
}

bool Model::Init(const char* mesh_filename, const char* texture_filename)
{
    if (!InitMesh(mesh_filename))
    {
        return false;
    }

    if (!InitTexture(texture_filename))
    {
        return false;
    }

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
    // TODO: Clean this up with operator overloads
    world_matrix_ = MatrixMultiply(MatrixIdentity(), MatrixTranslation(pos_.x, pos_.y, pos_.z));
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

Vector3 Model::GetPos()
{
    return pos_;
}

Matrix Model::GetWorldMatrix()
{
    return world_matrix_;
}

void Model::SetPos(float x, float y, float z)
{
    pos_ = Vector3(x, y, z);
}

bool Model::InitMesh(const char* filename)
{
    mesh_ = std::unique_ptr<Mesh>(new Mesh);

    if (mesh_ == nullptr)
    {
        return false;
    }

    if (!mesh_->Init(filename, true))
    {
        return false;
    }

    return true;
}

void Model::FinishMesh()
{
    if (mesh_)
    {
        mesh_->Finish();
    }
    return;
}

bool Model::InitTexture(const char* filename)
{
    texture_ = std::unique_ptr<Texture>(new Texture);

    if (!texture_)
    {
        return false;
    }

    if (!texture_->Init(filename))
    {
        return false;
    }

    return true;
}

void Model::FinishTexture()
{
    if (texture_)
    {
        texture_->Finish();
    }
    return;
}