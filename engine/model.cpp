#include "model.h"

Model::Model()
{
    mesh_ = nullptr;
    diffuse_texture_ = nullptr;
    normal_texture_ = nullptr;
    light_texture_ = nullptr;
    pos_ = Vector3(0.0f, 0.0f, 0.0f);
    world_matrix_ = MatrixIdentity();
}

Model::~Model()
{
}

bool Model::Init(const char* mesh_filename)
{
    MeshImporter mesh_data;
    if (!mesh_data.Load(mesh_filename, true))
    {
        return false;
    }
    mesh_ = std::unique_ptr<Mesh>(new Mesh);

    if (mesh_ == nullptr)
    {
        return false;
    }

    if (!mesh_->Init(&mesh_data))
    {
        return false;
    }

    // TODO: replace this with proper filesystem class
    std::string tex_folder(mesh_filename);
    // Go from folder/mesh/ to folder/
    tex_folder = tex_folder.substr(0, tex_folder.find_last_of('/'));
    tex_folder = tex_folder.substr(0, tex_folder.find_last_of('/'));
    tex_folder += "/tex/";
    for (const auto& tex : mesh_data.textures())
    {
        auto texture = std::unique_ptr<Texture>(new Texture);

        if (texture == nullptr)
        {
            return false;
        }

        std::string tex_file;
        tex_file = tex_folder + tex.filename;
        if (!texture->Init(tex_file.c_str(), tex.type))
        {
            return false;
        }

        // Transfer new texture to aproppriate member
        if (tex.type == Texture::DIFFUSE)
        {
            diffuse_texture_ = std::move(texture);
        }

        else if (tex.type == Texture::NORMAL)
        {
            normal_texture_ = std::move(texture);
        }

        else if (tex.type == Texture::LIGHT)
        {
            light_texture_ = std::move(texture);
        }
    }
    // TODO: make a proper solution for no diffuse texture
    if (diffuse_texture_ == nullptr)
    {
        diffuse_texture_ = std::unique_ptr<Texture>(new Texture);
        if (!diffuse_texture_->Init("../notes/me.dds", Texture::DIFFUSE))
        {
            return false;
        }
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
    // TODO: getters for all types of textures
    return diffuse_texture_->GetTexture();
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

bool Model::InitTexture(const char* filename, Texture::Type type)
{
    return true;
}

void Model::FinishTexture()
{
    if (diffuse_texture_)
    {
        diffuse_texture_->Finish();
    }
    return;
}