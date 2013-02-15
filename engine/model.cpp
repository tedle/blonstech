#include "model.h"

CModel::CModel()
{
    m_vertexBuffer = NULL;
    m_indexBuffer = NULL;
}

CModel::~CModel()
{
}

bool CModel::Init(ID3D11Device * device)
{
    return InitBuffers(device);
}

void CModel::Finish()
{
    FinishBuffers();

    return;
}

void CModel::Render(ID3D11DeviceContext* deviceContext)
{
    RenderBuffers(deviceContext);

    return;
}

int CModel::GetIndexCount()
{
    return m_indexCount;
}

bool CModel::InitBuffers(ID3D11Device* device)
{
    // TODO: make this actually load models
    Vertex* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    // TODO: temp tri gen
    m_vertexCount = m_indexCount = 3;
    vertices = new Vertex[m_vertexCount];
    if(!vertices)
        return false;

    indices = new unsigned long[m_indexCount];
    if(!indices)
        return false;

    vertices[0].pos    = XMFLOAT3(-1.0f, -1.0f, 0.0f);
    vertices[0].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[1].pos    = XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[1].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].pos    = XMFLOAT3(1.0f, -1.0f, 0.0f);
    vertices[2].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    for(int i = 0; i < m_indexCount; i++)
        indices[i] = i;

    // Vertex buffer desc
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Setup subresource pointer
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result))
        return false;

    // Index buffer desc :(
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Subrc pointer
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if(FAILED(result))
        return false;

    delete [] vertices;
    delete [] indices;

    return true;
}

void CModel::FinishBuffers()
{
    if(m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = NULL;
    }
    
    if(m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = NULL;
    }

    return;
}

void CModel::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride, offset;

    stride = sizeof(Vertex);
    offset = 0;

    // Vertex ACTIVE.....
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Indice.. ACTIVE!!!!
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // We do tris here
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}