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

bool Model::Init(const char* mesh_filename, RenderContext& context)
{
    g_log->Debug("Loading %s... ", mesh_filename);
    DWORD start = GetTickCount();

    MeshImporter mesh_data;
    if (!mesh_data.Load(mesh_filename, true))
    {
        g_log->Debug("[FAILED]\n");
        return false;
    }
    DWORD end = GetTickCount();
    g_log->Debug("[%ims]\n", end - start);

    mesh_ = std::unique_ptr<Mesh>(new Mesh);

    if (mesh_ == nullptr)
    {
        return false;
    }

    if (!mesh_->Init(&mesh_data, context))
    {
        return false;
    }

    start = GetTickCount();
    g_log->Debug("Loading textures... ");
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
        if (!texture->Init(tex_file.c_str(), tex.type, context))
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
        if (!diffuse_texture_->Init("../notes/me.dds", Texture::DIFFUSE, context))
        {
            return false;
        }
    }
    end = GetTickCount();
    g_log->Debug("[%ims]\n", end - start);

    return true;
}

void Model::Finish(RenderContext& context)
{
    FinishMesh(context);
    FinishTexture(context);

    return;
}

void Model::Render(RenderContext& context)
{
    // TODO: Clean this up with operator overloads
    world_matrix_ = MatrixMultiply(MatrixIdentity(), MatrixTranslation(pos_.x, pos_.y, pos_.z));
    context->SetModelBuffer(mesh_->vertex_buffer(), mesh_->index_buffer());

    return;
}

int Model::index_count()
{
    return mesh_->index_count();
}

TextureResource* Model::texture()
{
    // TODO: getters for all types of textures
    return diffuse_texture_->texture();
}

Vector3 Model::pos()
{
    return pos_;
}

Matrix Model::world_matrix()
{
    return world_matrix_;
}

void Model::set_pos(float x, float y, float z)
{
    pos_ = Vector3(x, y, z);
}

bool Model::InitMesh(const char* filename)
{

    return true;
}

void Model::FinishMesh(RenderContext& context)
{
    if (mesh_)
    {
        mesh_->Finish(context);
    }
    return;
}

bool Model::InitTexture(const char* filename, Texture::Type type)
{
    return true;
}

void Model::FinishTexture(RenderContext& context)
{
    if (diffuse_texture_)
    {
        diffuse_texture_->Finish(context);
    }
    return;
}