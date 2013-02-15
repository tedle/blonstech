#include "shader.h"

Shader::Shader()
{
    vertex_shader_ = NULL;
    pixel_shader_ = NULL;
    layout_ = NULL;
    matrix_buffer_ = NULL;
}

Shader::~Shader()
{
}

bool Shader::Init(ID3D11Device* device, HWND hwnd)
{
    return InitShader(device, hwnd, L"test.vert.fx", L"test.frag.fx");
}

void Shader::Finish()
{
    FinishShader();

    return;
}

bool Shader::Render(ID3D11DeviceContext* device_context, int index_count,
                     XMFLOAT4X4 world_matrix, XMFLOAT4X4 view_matrix, XMFLOAT4X4 projection_matrix)
{
    if (!SetShaderParams(device_context, world_matrix, view_matrix, projection_matrix))
        return false;

    RenderShader(device_context, index_count);

    return true;
}

bool Shader::InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vertex_filename, WCHAR* pixel_filename)
{
    HRESULT result;
    ID3D10Blob* error_message;
    ID3D10Blob* vertex_shader_buffer;
    ID3D10Blob* pixel_shader_buffer;
    D3D11_INPUT_ELEMENT_DESC input_layout[2];
    unsigned int num_elements;
    D3D11_BUFFER_DESC matrix_buffer_desc;

    error_message = NULL;
    vertex_shader_buffer = NULL;
    pixel_shader_buffer = NULL;

    // Compile vertex and pixel shaders
    result = D3DCompileFromFile(vertex_filename, NULL, NULL, "VertShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertex_shader_buffer, &error_message);
    if (FAILED(result))
    {
        if (error_message)
            OutputShaderErrorMessage(error_message, hwnd, vertex_filename);
        else
            MessageBox(hwnd, vertex_filename, L"Missing vert shader", MB_OK);

        return false;
    }

    result = D3DCompileFromFile(pixel_filename, NULL, NULL, "FragShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixel_shader_buffer, &error_message);
    if (FAILED(result))
    {
        if (error_message)
            OutputShaderErrorMessage(error_message, hwnd, pixel_filename);
        else
            MessageBox(hwnd, pixel_filename, L"Missing pixel shader", MB_OK);

        return false;
    }

    result = device->CreateVertexShader(vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize(), NULL, &vertex_shader_);
    if (FAILED(result))
        return false;

    result = device->CreatePixelShader(pixel_shader_buffer->GetBufferPointer(), pixel_shader_buffer->GetBufferSize(), NULL, &pixel_shader_);
    if (FAILED(result))
        return false;

    // Setup semantics
    input_layout[0].SemanticName = "POSITION";
    input_layout[0].SemanticIndex = 0;
    input_layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    input_layout[0].InputSlot = 0;
    input_layout[0].AlignedByteOffset = 0;
    input_layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    input_layout[0].InstanceDataStepRate = 0;

    input_layout[1].SemanticName = "COLOUR";
    input_layout[1].SemanticIndex = 0;
    input_layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    input_layout[1].InputSlot = 0;
    input_layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    input_layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    input_layout[1].InstanceDataStepRate = 0;

    num_elements = sizeof(input_layout) / sizeof(input_layout[0]);

    result = device->CreateInputLayout(input_layout, num_elements, vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize(), &layout_);
    if (FAILED(result))
        return false;

    vertex_shader_buffer->Release();
    pixel_shader_buffer->Release();

    // Setup constant buffers (sets vars yall)
    matrix_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    matrix_buffer_desc.ByteWidth = sizeof(MatrixBuffer);
    matrix_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrix_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrix_buffer_desc.MiscFlags = 0;
    matrix_buffer_desc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrix_buffer_desc, NULL, &matrix_buffer_);
    if (FAILED(result))
        return false;

    return true;
}

void Shader::FinishShader()
{
    if (matrix_buffer_)
    {
        matrix_buffer_->Release();
        matrix_buffer_ = NULL;
    }

    if (layout_)
    {
        layout_->Release();
        layout_ = NULL;
    }

    if (pixel_shader_)
    {
        pixel_shader_->Release();
        pixel_shader_ = NULL;
    }

    if (vertex_shader_)
    {
        vertex_shader_->Release();
        vertex_shader_ = NULL;
    }

    return;
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* error_message, HWND hwnd, WCHAR* shader_filename)
{
    char* compile_errors;
    unsigned long buffer_size;
    std::ofstream fout;

    compile_errors = (char*)(error_message->GetBufferPointer());
    buffer_size = error_message->GetBufferSize();

    fout.open("shader.log");

    for(unsigned int i = 0; i < buffer_size; i++)
        fout << compile_errors[i];

    fout.close();

    error_message->Release();

    MessageBox(hwnd, L"Failed to compile shader", shader_filename, MB_OK);
    
    return;
}

bool Shader::SetShaderParams(ID3D11DeviceContext* device_context,
                              XMFLOAT4X4 world_matrix, XMFLOAT4X4 view_matrix, XMFLOAT4X4 projection_matrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    MatrixBuffer* data;
    unsigned int num_buffers;

    // Transpose matrices, required in DX11
    XMStoreFloat4x4(&world_matrix, XMMatrixTranspose(XMLoadFloat4x4(&world_matrix)));
    XMStoreFloat4x4(&view_matrix, XMMatrixTranspose(XMLoadFloat4x4(&view_matrix)));
    XMStoreFloat4x4(&projection_matrix, XMMatrixTranspose(XMLoadFloat4x4(&projection_matrix)));

    // Lock buffer to gain write access
    result = device_context->Map(matrix_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    if (FAILED(result))
        return false;

    // Cast cbuffer to matrix
    data = (MatrixBuffer*)mapped_resource.pData;

    // Copy new matrix data
    data->world = world_matrix;
    data->view = view_matrix;
    data->projection = projection_matrix;

    // Unmap we done writing
    device_context->Unmap(matrix_buffer_, 0);

    // Why is this necessary
    num_buffers = 0;

    // Push the updated matrices
    device_context->VSSetConstantBuffers(num_buffers, 1, &matrix_buffer_);

    return true;
}

void Shader::RenderShader(ID3D11DeviceContext* device_context, int index_count)
{
    // Make sure we shove them verts in so they fit rite
    device_context->IASetInputLayout(layout_);

    // our shader!!!!
    device_context->VSSetShader(vertex_shader_, NULL, 0);
    device_context->PSSetShader(pixel_shader_, NULL, 0);

    // wow here it is. its so small. did you think itd be this small?
    device_context->DrawIndexed(index_count, 0, 0);

    return;
}