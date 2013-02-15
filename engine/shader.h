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

class CShader
{
private:
    struct MatrixBuffer
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

public:
    CShader();
    ~CShader();

    bool Init(ID3D11Device*, HWND);
    void Finish();
    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

private:
    bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
    void FinishShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    bool SetShaderParams(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11Buffer* m_matrixBuffer;
};

#endif