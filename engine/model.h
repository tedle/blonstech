#ifndef __MODEL_H__
#define __MODEL_H__

// Includes
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

// Class
class Model
{
private:
    struct Vertex
    {
        XMFLOAT3 pos;
        XMFLOAT4 colour;
    };

public:
    Model();
    ~Model();

    bool Init(ID3D11Device*);
    void Finish();
    void Render(ID3D11DeviceContext*);

    int  GetIndexCount();

private:
    bool InitBuffers(ID3D11Device*);
    void FinishBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

    ID3D11Buffer *vertex_buffer_, *index_buffer_;
    int vertex_count_, index_count_;

};

#endif