#include "graphics/model.h"

// Includes
// TODO: remove this, only used for timing
#include <Windows.h>

// Local Includes
#include "graphics/mesh.h"
#include "graphics/meshimporter.h"
#include "math/math.h"

namespace blons
{
Model::Model(std::string mesh_filename, RenderContext& context)
{
    Init(mesh_filename, context);
}

void Model::Init(std::string mesh_filename, RenderContext& context)
{
    mesh_ = nullptr;
    diffuse_texture_ = nullptr;
    normal_texture_ = nullptr;
    light_texture_ = nullptr;
    pos_ = Vector3(0.0f, 0.0f, 0.0f);
    world_matrix_ = MatrixIdentity();

    g_log->Debug("Loading %s... ", mesh_filename);
    DWORD64 start = GetTickCount64();

    MeshImporter mesh_data(mesh_filename, true);
    DWORD64 end = GetTickCount64();
    g_log->Debug("[%ims]\n", end - start);

    mesh_ = std::unique_ptr<Mesh>(new Mesh(mesh_data, context));

    if (mesh_ == nullptr)
    {
        throw "Failed to initialize mesh";
    }

    g_log->Debug("Loading textures... ");
    start = GetTickCount64();
    // TODO: replace this with proper filesystem class
    std::string tex_folder(mesh_filename);
    // Go from folder/mesh/ to folder/
    tex_folder = tex_folder.substr(0, tex_folder.find_last_of('/'));
    tex_folder = tex_folder.substr(0, tex_folder.find_last_of('/'));
    tex_folder += "/tex/";
    for (const auto& tex : *mesh_data.textures())
    {
        std::string tex_file;
        tex_file = tex_folder + tex.filename;
        auto texture = std::unique_ptr<Texture>(new Texture(tex_file.c_str(), tex.type, context));

        if (texture == nullptr)
        {
            throw "Failed to load texture";
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
        diffuse_texture_ = std::unique_ptr<Texture>(new Texture("../../notes/me.dds",
                                                                Texture::DIFFUSE, context));
        if (diffuse_texture_ == nullptr)
        {
            throw "Failed to load diffuse texture";
        }
    }
    end = GetTickCount64();
    g_log->Debug("[%ims]\n", end - start);
}

Model::~Model()
{
}

void Model::Render(RenderContext& context)
{
    // TODO: Clean this up with operator overloads
    world_matrix_ = MatrixMultiply(MatrixIdentity(), MatrixTranslation(pos_.x, pos_.y, pos_.z));
    context->BindMeshBuffer(mesh_->vertex_buffer(), mesh_->index_buffer());

    return;
}

int Model::index_count() const
{
    return mesh_->index_count();
}

const TextureResource* Model::texture() const
{
    // TODO: getters for all types of textures
    return diffuse_texture_->texture();
}

Vector3 Model::pos() const
{
    return pos_;
}

Matrix Model::world_matrix() const
{
    return world_matrix_;
}

void Model::set_pos(float x, float y, float z)
{
    pos_ = Vector3(x, y, z);
}
} // namespace blons
