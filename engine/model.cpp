#include "model.h"

Model::Model()
{
    vertex_buffer_ = nullptr;
    index_buffer_ = nullptr;
    texture_ = nullptr;
}

Model::~Model()
{
}

bool Model::Init(WCHAR* texture_filename)
{
    if (!InitBuffers())
        return false;

    if (!LoadTexture(texture_filename))
        return false;

    return true;
}

void Model::Finish()
{
    ReleaseTexture();
    FinishBuffers();

    return;
}

void Model::Render()
{
    g_render->SetModelBuffer(vertex_buffer_, index_buffer_);

    return;
}

int Model::GetIndexCount()
{
    return index_count_;
}

TextureResource* Model::GetTexture()
{
    return texture_->GetTexture();
}

bool Model::InitBuffers()
{
    // TODO: make this actually load models
    Vertex* vertices;
    unsigned long* indices;

    vertex_buffer_ = static_cast<BufferResource*>(g_render->CreateBufferResource());
    index_buffer_  = static_cast<BufferResource*>(g_render->CreateBufferResource());

    vertex_count_ = index_count_ = 6;
    vertices = new Vertex[vertex_count_];
    if (!vertices)
        return false;

    indices = new unsigned long[index_count_];
    if (!indices)
        return false;

    vertices[0].pos.x = -1.0f;
    vertices[0].pos.y = -1.0f;
    vertices[0].pos.z = 0.0f;
    vertices[0].tex.x = 0.0f;
    vertices[0].tex.y = 1.0f;

    vertices[1].pos.x = -1.0f;
    vertices[1].pos.y = 1.0f;
    vertices[1].pos.z = 0.0f;
    vertices[1].tex.x = 0.0f;
    vertices[1].tex.y = 0.0f;

    vertices[2].pos.x = 1.0f;
    vertices[2].pos.y = 1.0f;
    vertices[2].pos.z = 0.0f;
    vertices[2].tex.x = 1.0f;
    vertices[2].tex.y = 0.0f;

    vertices[3].pos.x = 1.0f;
    vertices[3].pos.y = -1.0f;
    vertices[3].pos.z = 0.0f;
    vertices[3].tex.x = 1.0f;
    vertices[3].tex.y = 1.0f;
    
    vertices[4] = vertices[0];
    vertices[5] = vertices[2];

    for (int i = 0; i < index_count_; i++)
        indices[i] = i;

    if (!g_render->RegisterModel(vertex_buffer_, index_buffer_, vertices, vertex_count_,
                                 indices, index_count_))
        return false;

    delete [] vertices;
    delete [] indices;

    return true;
}

void Model::FinishBuffers()
{
    if (index_buffer_)
    {
        g_render->DestroyBufferResource(index_buffer_);
        index_buffer_ = nullptr;
    }
    
    if (vertex_buffer_)
    {
        g_render->DestroyBufferResource(vertex_buffer_);
        vertex_buffer_ = nullptr;
    }

    return;
}

bool Model::LoadTexture(WCHAR* filename)
{
    texture_ = new Texture;

    if (!texture_)
        return false;

    if (!texture_->Init(filename))
        return false;

    return true;
}

void Model::ReleaseTexture()
{
    if (texture_)
    {
        texture_->Finish();
        delete texture_;
        texture_ = nullptr;
    }
    return;
}