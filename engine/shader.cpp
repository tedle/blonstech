#include "shader.h"

CShader::CShader()
{
    m_vertexShader = NULL;
    m_pixelShader = NULL;
    m_layout = NULL;
    m_matrixBuffer = NULL;
}

CShader::~CShader()
{
}

bool CShader::Init(ID3D11Device* device, HWND hwnd)
{
    return InitShader(device, hwnd, L"test.vert.fx", L"test.frag.fx");
}

void CShader::Finish()
{
    FinishShader();

    return;
}

bool CShader::Render(ID3D11DeviceContext* deviceContext, int indexCount,
                     XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    if(!SetShaderParams(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
        return false;

    RenderShader(deviceContext, indexCount);

    return true;
}

bool CShader::InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vertFn, WCHAR* fragFn)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

    errorMessage = NULL;
    vertexShaderBuffer = NULL;
    pixelShaderBuffer = NULL;

    // Compile vertex and pixel shaders
    result = D3DCompileFromFile(vertFn, NULL, NULL, "VertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if(FAILED(result))
    {
        if(errorMessage)
            OutputShaderErrorMessage(errorMessage, hwnd, vertFn);
        else
            MessageBox(hwnd, vertFn, L"Missing vert shader", MB_OK);

        return false;
    }

    result = D3DCompileFromFile(fragFn, NULL, NULL, "PixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if(FAILED(result))
    {
        if(errorMessage)
            OutputShaderErrorMessage(errorMessage, hwnd, fragFn);
        else
            MessageBox(hwnd, fragFn, L"Missing pixel shader", MB_OK);

        return false;
    }

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if(FAILED(result))
        return false;

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if(FAILED(result))
        return false;

    // Setup semantics
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOUR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
    if(FAILED(result))
        return false;

    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();

    // Setup constant buffers (sets vars yall)
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if(FAILED(result))
        return false;

    return true;
}

void CShader::FinishShader()
{
    if(m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = NULL;
    }

    if(m_layout)
    {
        m_layout->Release();
        m_layout = NULL;
    }

    if(m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = NULL;
    }

    if(m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = NULL;
    }

    return;
}

void CShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFn)
{
    char* compileErrors;
    unsigned long bufferSize;
    std::ofstream fout;

    compileErrors = (char*)(errorMessage->GetBufferPointer());
    bufferSize = errorMessage->GetBufferSize();

    fout.open("shader.log");

    for(unsigned int i = 0; i < bufferSize; i++)
        fout << compileErrors[i];

    fout.close();

    errorMessage->Release();

    MessageBox(hwnd, L"Failed to compile shader", shaderFn, MB_OK);
    
    return;
}

bool CShader::SetShaderParams(ID3D11DeviceContext* deviceContext,
                              XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBuffer* data;
    unsigned int bufferNumber;

    // Transpose matrices, required in DX11
    worldMatrix      = XMMatrixTranspose(worldMatrix);
    viewMatrix       = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // Lock buffer to gain write access
    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
        return false;

    // Cast cbuffer to matrix
    data = (MatrixBuffer*)mappedResource.pData;

    // Copy new matrix data
    data->world = worldMatrix;
    data->view = viewMatrix;
    data->projection = projectionMatrix;

    // Unmap we done writing
    deviceContext->Unmap(m_matrixBuffer, 0);

    // Why is this necessary
    bufferNumber = 0;

    // Push the updated matrices
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    return true;
}

void CShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // Make sure we shove them verts in so they fit rite
    deviceContext->IASetInputLayout(m_layout);

    // our shader!!!!
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // wow here it is. its so small. did you think itd be this small?
    deviceContext->DrawIndexed(indexCount, 0, 0);

    return;
}