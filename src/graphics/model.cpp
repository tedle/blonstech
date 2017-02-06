////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

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
namespace
{
TextureType::Options kAlbedoOptions = { TextureType::AUTO, TextureType::LINEAR, TextureType::REPEAT };
TextureType::Options kNormalOptions = { TextureType::AUTO, TextureType::LINEAR, TextureType::REPEAT };
TextureType::Options kLightOptions =  { TextureType::RAW,  TextureType::LINEAR, TextureType::REPEAT };
}

Model::Model(std::string mesh_filename)
{
    mesh_ = nullptr;
    albedo_texture_ = nullptr;
    normal_texture_ = nullptr;
    light_texture_ = nullptr;
    pos_ = Vector3(0);
    scale_ = Vector3(1);
    world_matrix_ = MatrixIdentity();

    log::Debug("Loading %s... ", mesh_filename.c_str());
    Timer timer;

    timer.start();
    static Timer total_timer;
    total_timer.start();
    mesh_.reset(new Mesh(mesh_filename));
    total_timer.pause();
    log::Debug("[%ims(%ims)]\n", timer.ms(), total_timer.ms());


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
    for (const auto& tex : mesh_->textures())
    {
        std::string tex_file;
        tex_file = tex_folder + tex.filename;
        TextureType::Options tex_options;
        switch (tex.type)
        {
        case Mesh::TextureInfo::ALBEDO:
            tex_options = kAlbedoOptions;
            break;
        case Mesh::TextureInfo::NORMAL:
            tex_options = kNormalOptions;
            break;
        case Mesh::TextureInfo::LIGHT:
            tex_options = kLightOptions;
            break;
        default:
            throw "Unknown texture type in mesh file";
        }
        auto texture = std::unique_ptr<Texture>(new Texture(tex_file.c_str(), tex_options));

        if (texture == nullptr)
        {
            throw "Failed to load texture";
        }

        // Transfer new texture to aproppriate member
        if (tex.type == Mesh::TextureInfo::ALBEDO)
        {
            albedo_texture_ = std::move(texture);
        }
        else if (tex.type == Mesh::TextureInfo::NORMAL)
        {
            normal_texture_ = std::move(texture);
        }
        else if (tex.type == Mesh::TextureInfo::LIGHT)
        {
            light_texture_ = std::move(texture);
        }
    }
    if (albedo_texture_ == nullptr)
    {
        albedo_texture_.reset(new Texture("blons:none", kAlbedoOptions));
    }
    if (normal_texture_ == nullptr)
    {
        normal_texture_.reset(new Texture("blons:normal", kNormalOptions));
    }
    if (light_texture_ == nullptr)
    {
        light_texture_.reset(new Texture("blons:none", kLightOptions));
    }
    log::Debug("[%ims]\n", timer.ms());
}

void Model::Render()
{
    // TODO: Clean this up with operator overloads
    world_matrix_ = MatrixScale(scale_.x, scale_.y, scale_.z) * MatrixTranslation(pos_.x, pos_.y, pos_.z);
    render::context()->BindMeshBuffer(mesh_->vertex_buffer(), mesh_->index_buffer());
}

void Model::Reload()
{
    mesh_->Reload();
    albedo_texture_->Reload();
    normal_texture_->Reload();
    light_texture_->Reload();
}

int Model::index_count() const
{
    return mesh_->index_count();
}

const MeshData& Model::mesh() const
{
    return mesh_->mesh();
}

const TextureResource* Model::albedo() const
{
    return albedo_texture_->texture();
}

const TextureResource* Model::normal() const
{
    return normal_texture_->texture();
}

const TextureResource* Model::lightmap() const
{
    return light_texture_->texture();
}

Vector3 Model::pos() const
{
    return pos_;
}

Vector3 Model::scale() const
{
    return scale_;
}

Matrix Model::world_matrix() const
{
    return world_matrix_;
}

void Model::set_pos(units::world x, units::world y, units::world z)
{
    pos_ = Vector3(x, y, z);
}

void Model::set_scale(units::world x, units::world y, units::world z)
{
    scale_ = Vector3(x, y, z);
}
} // namespace blons
