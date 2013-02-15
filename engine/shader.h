#ifndef __SHADER_H__
#define __SHADER_H__

// TODO: get rid of this dependancy eventually
#pragma comment(lib, "d3dcompiler.lib")

// Includes
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>

using namespace DirectX;

class Shader
{
private:
    struct MatrixBuffer
    {
        XMFLOAT4X4 world;
        XMFLOAT4X4 view;
        XMFLOAT4X4 projection;
    };

public:
    Shader();
    ~Shader();

    bool Init(ID3D11Device*, HWND);
    void Finish();
    bool Render(ID3D11DeviceContext*, int, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4);

private:
    bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void FinishShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    bool SetShaderParams(ID3D11DeviceContext*, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* vertex_shader_;
    ID3D11PixelShader* pixel_shader_;
    ID3D11InputLayout* layout_;
    ID3D11Buffer* matrix_buffer_;
};

#endif