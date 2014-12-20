#include <blons/graphics/model.h>

// Includes
// TODO: remove this, only used for timing
#include <Windows.h>

// Public Includes
#include <blons/graphics/mesh.h>
#include <blons/graphics/meshimporter.h>
#include <blons/math/math.h>
#include <blons/system/timer.h>

namespace blons
{
Model::Model(std::string mesh_filename, RenderContext& context)
{
    mesh_ = nullptr;
    diffuse_texture_ = nullptr;
    normal_texture_ = nullptr;
    light_texture_ = nullptr;
    pos_ = Vector3(0.0f, 0.0f, 0.0f);
    world_matrix_ = MatrixIdentity();

    log::Debug("Loading %s... ", mesh_filename.c_str());
    Timer timer;

    timer.start();
    MeshImporter mesh(mesh_filename, true);
    log::Debug("[%ims]\n", timer.ms());

    mesh_.reset(new Mesh(*mesh.mesh_data(), context));

    if (mesh_ == nullptr)
    {
        throw "Failed to initialize mesh";
    }

    log::Debug("Loading textures... ");
    timer.start();
    // TODO: replace this with proper filesystem class
    std::string tex_folder(mesh_filename);
    // Go from folder/mesh/ to folder/
    tex_folder = tex_folder.substr(0, tex_folder.find_last_of('/'));
    tex_folder = tex_folder.substr(0, tex_folder.find_last_of('/'));
    tex_folder += "/tex/";
    for (const auto& tex : *mesh.textures())
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
        diffuse_texture_.reset(new Texture("../../notes/me.dds", Texture::DIFFUSE, context));
        if (diffuse_texture_ == nullptr)
        {
            throw "Failed to load diffuse texture";
        }
    }
    log::Debug("[%ims]\n", timer.ms());
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

void Model::set_pos(units::world x, units::world y, units::world z)
{
    pos_ = Vector3(x, y, z);
}
} // namespace blons
