#ifndef __MODEL_H__
#define __MODEL_H__

// Includes
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

// Class
class CModel
{
private:
    struct Vertex
    {
        XMFLOAT3 pos;
        XMFLOAT4 colour;
    };

public:
    CModel();
    ~CModel();

    bool Init(ID3D11Device*);
    void Finish();
    void Render(ID3D11DeviceContext*);

    int GetIndexCount();

private:
    bool InitBuffers(ID3D11Device*);
    void FinishBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_vertexCount, m_indexCount;

};

#endif