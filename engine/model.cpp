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

bool Model::Init(ID3D11Device * device, WCHAR* texture_filename)
{
    if (!InitBuffers(device))
        return false;

    if (!LoadTexture(device, texture_filename))
        return false;

    return true;
}

void Model::Finish()
{
    ReleaseTexture();
    FinishBuffers();

    return;
}

void Model::Render(ID3D11DeviceContext* device_context)
{
    RenderBuffers(device_context);

    return;
}

int Model::GetIndexCount()
{
    return index_count_;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
    return texture_->GetTexture();
}

bool Model::InitBuffers(ID3D11Device* device)
{
    // TODO: make this actually load models
    Vertex* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;
    HRESULT result;

    // TODO: temp tri gen
    vertex_count_ = index_count_ = 6;
    vertices = new Vertex[vertex_count_];
    if (!vertices)
        return false;

    indices = new unsigned long[index_count_];
    if (!indices)
        return false;

    vertices[0].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
    vertices[0].tex = XMFLOAT2(0.0f, 1.0f);

    vertices[1].pos = XMFLOAT3(-1.0f, 1.0f, 0.0f);
    vertices[1].tex = XMFLOAT2(0.0f, 0.0f);

    vertices[2].pos = XMFLOAT3(1.0f, 1.0f, 0.0f);
    vertices[2].tex = XMFLOAT2(1.0f, 0.0f);

    vertices[3].pos = XMFLOAT3(1.0f, -1.0f, 0.0f);
    vertices[3].tex = XMFLOAT2(1.0f, 1.0f);
    
    vertices[4].pos = vertices[0].pos;
    vertices[4].tex = vertices[0].tex;
    
    vertices[5].pos = vertices[2].pos;
    vertices[5].tex = vertices[2].tex;

    for (int i = 0; i < index_count_; i++)
        indices[i] = i;

    // Vertex buffer desc
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(Vertex) * vertex_count_;
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;

    // Setup subresource pointer
    vertex_data.pSysMem = vertices;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer_);
    if (FAILED(result))
        return false;

    // Index buffer desc :(
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = sizeof(unsigned long) * index_count_;
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags = 0;
    index_buffer_desc.MiscFlags = 0;
    index_buffer_desc.StructureByteStride = 0;

    // Subrc pointer
    index_data.pSysMem = indices;
    index_data.SysMemPitch = 0;
    index_data.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&index_buffer_desc, &index_data, &index_buffer_);
    if (FAILED(result))
        return false;

    delete [] vertices;
    delete [] indices;

    return true;
}

void Model::FinishBuffers()
{
    if (index_buffer_)
    {
        index_buffer_->Release();
        index_buffer_ = nullptr;
    }
    
    if (vertex_buffer_)
    {
        vertex_buffer_->Release();
        vertex_buffer_ = nullptr;
    }

    return;
}

void Model::RenderBuffers(ID3D11DeviceContext* device_context)
{
    unsigned int stride, offset;

    stride = sizeof(Vertex);
    offset = 0;

    // Vertex ACTIVE.....
    device_context->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);

    // Indice.. ACTIVE!!!!
    device_context->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);

    // We do tris here
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}

bool Model::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
    texture_ = new Texture;

    if (!texture_)
        return false;

    if (!texture_->Init(device, filename))
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