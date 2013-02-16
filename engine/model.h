#ifndef BLONSTECH_MODEL_H_
#define BLONSTECH_MODEL_H_

// Includes
#include <d3d11.h>
#include <DirectXMath.h>
// Local Includes
#include "texture.h"

using namespace DirectX;

// Class
class Model
{
private:
    struct Vertex
    {
        XMFLOAT3 pos;
        XMFLOAT2 tex;
    };

public:
    Model();
    ~Model();

    bool Init(ID3D11Device*, WCHAR*);
    void Finish();
    void Render(ID3D11DeviceContext*);

    int  GetIndexCount();

    ID3D11ShaderResourceView* GetTexture();

private:
    bool InitBuffers(ID3D11Device*);
    void FinishBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

    bool LoadTexture(ID3D11Device*, WCHAR*);
    void ReleaseTexture();

    ID3D11Buffer *vertex_buffer_, *index_buffer_;
    int vertex_count_, index_count_;
    Texture* texture_;
};

#endif