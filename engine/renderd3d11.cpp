#include "renderd3d11.h"

RenderD3D11::RenderD3D11()
{
    swapchain_ = nullptr;
    device_ = nullptr;
    device_context_ = nullptr;
    render_target_view_ = nullptr;
    depth_stencil_buffer_ = nullptr;
    depth_stencil_state_ = nullptr;
    depth_stencil_view_ = nullptr;
    raster_state_ = nullptr;
}

RenderD3D11::~RenderD3D11()
{
}

bool RenderD3D11::Init(int screen_width, int screen_height, bool vsync, HWND hwnd, bool fullscreen, float screen_depth, float screen_near)
{
    HRESULT result;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapter_out;
    unsigned int num_modes, numerator, denominator, string_len;
    DXGI_MODE_DESC* display_modes;
    DXGI_ADAPTER_DESC adapter_desc;
    DXGI_SWAP_CHAIN_DESC swapchain_desc;
    D3D_FEATURE_LEVEL feature_level;
    ID3D11Texture2D* back_buffer;
    D3D11_TEXTURE2D_DESC depth_buffer_desc;
    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
    D3D11_RASTERIZER_DESC raster_desc;
    D3D11_VIEWPORT viewport;
    float fov, screen_aspect;

    vsync_ = vsync;

    // Get a interface
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result))
        return false;

    // Get a video card
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result))
        return false;

    // Sort thru viddy cards
    result = adapter->EnumOutputs(0, &adapter_out);
    if (FAILED(result))
        return false;

    // Get only the best of the display modess
    result = adapter_out->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, nullptr);
    if (FAILED(result))
        return false;

    // Allocate a sucker to find what modes we g0t
    display_modes = new DXGI_MODE_DESC[num_modes];
    if (!display_modes)
        return false;

    // Stuff that sucker up!!!
    result = adapter_out->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, display_modes);
    if (FAILED(result))
        return false;

    // Now to actually find one compatible w/ what i got
    for (unsigned int i = 0; i < num_modes; i++)
    {
        if (display_modes[i].Width  == (unsigned int)screen_width
        && display_modes[i].Height == (unsigned int)screen_height)
        {
            numerator = display_modes[i].RefreshRate.Numerator;
            denominator = display_modes[i].RefreshRate.Denominator;
        }
    }

    // Tell me bout vid card
    result = adapter->GetDesc(&adapter_desc);
    if (FAILED(result))
        return false;

    video_card_memory_ = (int)(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

    if (wcstombs_s(&string_len, video_card_desc_, 128, adapter_desc.Description, 128))
        return false;

    // Clean up some mamorie
    delete [] display_modes;
    display_modes = nullptr;

    adapter_out->Release();
    adapter_out = nullptr;

    adapter->Release();
    adapter = nullptr;

    factory->Release();
    factory = nullptr;

    // Init swap chain
    ZeroMemory(&swapchain_desc, sizeof(swapchain_desc));

    // Triple buffering blows
    swapchain_desc.BufferCount = 1;

    swapchain_desc.BufferDesc.Width = screen_width;
    swapchain_desc.BufferDesc.Height = screen_height;

    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    swapchain_desc.BufferDesc.RefreshRate.Numerator   = vsync_ ? numerator   : 0;
    swapchain_desc.BufferDesc.RefreshRate.Denominator = vsync_ ? denominator : 1;

    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    swapchain_desc.OutputWindow = hwnd;

    // No multisampling ...
    swapchain_desc.SampleDesc.Count = 1;
    swapchain_desc.SampleDesc.Quality = 0;

    swapchain_desc.Windowed = !fullscreen;

    swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    swapchain_desc.Flags = 0;

    // 11.1 can fuck right off
    feature_level = D3D_FEATURE_LEVEL_11_0;

    // FINALLY initialize directx holey shit
    result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &feature_level, 1, D3D11_SDK_VERSION,
                                           &swapchain_desc, &swapchain_, &device_, nullptr, &device_context_);
    if (FAILED(result))
        return false;

    result = swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
    if (FAILED(result))
        return false;

    // Init the render view
    result = device_->CreateRenderTargetView(back_buffer, nullptr, &render_target_view_);
    if (FAILED(result))
        return false;

    back_buffer->Release();
    back_buffer = nullptr;

    // Init depth + stencil buffer
    ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));

    depth_buffer_desc.Width = screen_width;
    depth_buffer_desc.Height = screen_height;
    depth_buffer_desc.MipLevels = 1;
    depth_buffer_desc.ArraySize = 1;
    depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buffer_desc.SampleDesc.Count = 1;
    depth_buffer_desc.SampleDesc.Quality = 0;
    depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_desc.CPUAccessFlags = 0;
    depth_buffer_desc.MiscFlags = 0;

    result = device_->CreateTexture2D(&depth_buffer_desc, nullptr, &depth_stencil_buffer_);
    if (FAILED(result))
        return false;

    // How thhe stencil work
    ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));

    depth_stencil_desc.DepthEnable = true;
    depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

    depth_stencil_desc.StencilEnable = true;
    depth_stencil_desc.StencilReadMask = 0xFF;
    depth_stencil_desc.StencilWriteMask = 0xFF;

    depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state_);
    if (FAILED(result))
        return false;

    // Finally plug it in
    device_context_->OMSetDepthStencilState(depth_stencil_state_, 1);

    // AND teach dirx how to use depth buffer
    ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));

    depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;

    result = device_->CreateDepthStencilView(depth_stencil_buffer_, &depth_stencil_view_desc, &depth_stencil_view_);
    if (FAILED(result))
        return false;

    // attach 2 pipeline
    device_context_->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);

    // Setup personal rasterizer state for tinkering mayb...
    raster_desc.AntialiasedLineEnable = false;
    // TODO: REENABLE BACKFACE CULLING
    raster_desc.CullMode = D3D11_CULL_NONE; // Backface culling SO ftw...
    raster_desc.DepthBias = 0;
    raster_desc.DepthBiasClamp = 0.0f;
    raster_desc.DepthClipEnable = true;
    raster_desc.FillMode = D3D11_FILL_SOLID;
    raster_desc.FrontCounterClockwise = false;
    raster_desc.MultisampleEnable = false;
    raster_desc.ScissorEnable = false;
    raster_desc.SlopeScaledDepthBias = 0.0f;

    result = device_->CreateRasterizerState(&raster_desc, &raster_state_);
    if (FAILED(result))
        return false;

    // and apply!
    device_context_->RSSetState(raster_state_);

    // Time to get to work on the viewport!!!! (oof exciting)
    viewport.Width = (float)screen_width;
    viewport.Height = (float)screen_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    device_context_->RSSetViewports(1, &viewport);

    // Projection matrix (3D space->2D screen)
    fov = kPi / 4.0f;
    screen_aspect = (float)screen_width / (float)screen_height;

    proj_matrix_ = MatrixPerspectiveFov(fov, screen_aspect, screen_near, screen_depth);

    // World matrix (models->3D space) shouldnt this be in model class??
    // TODO: Make this part of model class, maybe separate world/view+proj matrix cbuffers, and update world matrix for each model rendered
    world_matrix_ = MatrixIdentity();

    // Ortho projection matrix (for 2d stuff)
    ortho_matrix_ = MatrixOrthographic(screen_width, screen_height, screen_near, screen_depth);

    return true;
}

void RenderD3D11::Finish()
{
    // Always shut down a swapchain in windowed mode or problems happen
    if (swapchain_)
        swapchain_->SetFullscreenState(false, nullptr);

    if (raster_state_)
    {
        raster_state_->Release();
        raster_state_ = nullptr;
    }

    if (depth_stencil_view_)
    {
        depth_stencil_view_->Release();
        depth_stencil_view_ = nullptr;
    }

    if (depth_stencil_state_)
    {
        depth_stencil_state_->Release();
        depth_stencil_state_ = nullptr;
    }

    if (depth_stencil_buffer_)
    {
        depth_stencil_buffer_->Release();
        depth_stencil_buffer_ = nullptr;
    }

    if (render_target_view_)
    {
        render_target_view_->Release();
        render_target_view_ = nullptr;
    }

    if (device_context_)
    {
        device_context_->Release();
        device_context_ = nullptr;
    }

    if (device_)
    {
        device_->Release();
        device_ = nullptr;
    }

    if (swapchain_)
    {
        swapchain_->Release();
        swapchain_ = 0;
    }

    return;
}

void RenderD3D11::BeginScene()
{
    float colour[4];

    colour[0] = 1.0f;
    colour[1] = 0.0f;
    colour[2] = 1.0f;
    colour[3] = 1.0f;

    device_context_->ClearRenderTargetView(render_target_view_, colour);

    device_context_->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH, 1.0f, 0);

    return;
}

void RenderD3D11::EndScene()
{
    swapchain_->Present(vsync_, 0);

    return;
}

void* RenderD3D11::CreateBufferResource()
{
    return malloc(sizeof(BufferResourceD3D11));
}

void* RenderD3D11::CreateTextureResource()
{
    return malloc(sizeof(TextureResourceD3D11));
}

void* RenderD3D11::CreateShaderResource()
{
    return malloc(sizeof(ShaderResourceD3D11));
}

void RenderD3D11::DestroyBufferResource(BufferResource* buffer)
{
    BufferResourceD3D11* buf = static_cast<BufferResourceD3D11*>(buffer);
    buf->p->Release();
    delete buf;
}

void RenderD3D11::DestroyTextureResource(TextureResource* texture)
{
    TextureResourceD3D11* tex = static_cast<TextureResourceD3D11*>(texture);
    tex->p->Release();
    delete tex;
}

void RenderD3D11::DestroyShaderResource(ShaderResource* shader)
{
    ShaderResourceD3D11* p = static_cast<ShaderResourceD3D11*>(shader);

    if (p->sampler_state_)
    {
        p->sampler_state_->Release();
        p->sampler_state_ = nullptr;
    }

    if (p->matrix_buffer_)
    {
        p->matrix_buffer_->Release();
        p->matrix_buffer_ = nullptr;
    }

    if (p->layout_)
    {
        p->layout_->Release();
        p->layout_ = nullptr;
    }

    if (p->pixel_shader_)
    {
        p->pixel_shader_->Release();
        p->pixel_shader_ = nullptr;
    }

    if (p->vertex_shader_)
    {
        p->vertex_shader_->Release();
        p->vertex_shader_ = nullptr;
    }

    delete p;

    return;
}

bool RenderD3D11::RegisterModel(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                Vertex* vertices, unsigned int vert_count,
                                unsigned long* indices, unsigned int index_count)
{
    BufferResourceD3D11* vert_buf  = static_cast<BufferResourceD3D11*>(vertex_buffer);
    BufferResourceD3D11* index_buf = static_cast<BufferResourceD3D11*>(index_buffer);

    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;
    HRESULT result;

    // Vertex buffer desc
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(Vertex) * vert_count;
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;

    // Setup subresource pointer
    vertex_data.pSysMem = vertices;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;

    result = device_->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vert_buf->p);
    if (FAILED(result))
        return false;

    // Index buffer desc :(
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = sizeof(unsigned long) * index_count;
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags = 0;
    index_buffer_desc.MiscFlags = 0;
    index_buffer_desc.StructureByteStride = 0;

    // Subrc pointer
    index_data.pSysMem = indices;
    index_data.SysMemPitch = 0;
    index_data.SysMemSlicePitch = 0;

    result = device_->CreateBuffer(&index_buffer_desc, &index_data, &index_buf->p);
    if (FAILED(result))
        return false;

    return true;
}

void RenderD3D11::RegisterTexture()
{
    return;
}

bool RenderD3D11::RegisterShader(ShaderResource* program, WCHAR* vertex_filename, WCHAR* pixel_filename)
{
    ShaderResourceD3D11* shader = static_cast<ShaderResourceD3D11*>(program);

    HRESULT result;
    ID3D10Blob* error_message;
    ID3D10Blob* vertex_shader_buffer;
    ID3D10Blob* pixel_shader_buffer;
    D3D11_INPUT_ELEMENT_DESC input_layout[2];
    unsigned int num_elements;
    D3D11_BUFFER_DESC matrix_buffer_desc;
    D3D11_SAMPLER_DESC sample_desc;

    error_message = nullptr;
    vertex_shader_buffer = nullptr;
    pixel_shader_buffer = nullptr;

    // Compile vertex and pixel shaders
    result = D3DCompileFromFile(vertex_filename, nullptr, nullptr, "VertShader", "vs_5_0",
                                D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertex_shader_buffer, &error_message);
    if (FAILED(result))
    {
        if (error_message)
            OutputShaderErrorMessage(error_message);

        return false;
    }

    result = device_->CreateVertexShader(vertex_shader_buffer->GetBufferPointer(),
                                         vertex_shader_buffer->GetBufferSize(), nullptr, &shader->vertex_shader_);
    if (FAILED(result))
        return false;

    result = D3DCompileFromFile(pixel_filename, nullptr, nullptr, "FragShader", "ps_5_0",
                                D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixel_shader_buffer, &error_message);
    if (FAILED(result))
    {
        if (error_message)
            OutputShaderErrorMessage(error_message);

        return false;
    }

    result = device_->CreatePixelShader(pixel_shader_buffer->GetBufferPointer(),
                                        pixel_shader_buffer->GetBufferSize(), nullptr, &shader->pixel_shader_);
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

    input_layout[1].SemanticName = "TEXCOORD";
    input_layout[1].SemanticIndex = 0;
    input_layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    input_layout[1].InputSlot = 0;
    input_layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    input_layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    input_layout[1].InstanceDataStepRate = 0;

    num_elements = sizeof(input_layout) / sizeof(input_layout[0]);

    result = device_->CreateInputLayout(input_layout, num_elements,
                                       vertex_shader_buffer->GetBufferPointer(),
                                       vertex_shader_buffer->GetBufferSize(),
                                       &shader->layout_);
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

    result = device_->CreateBuffer(&matrix_buffer_desc, nullptr, &shader->matrix_buffer_);
    if (FAILED(result))
        return false;

    // Setup the sampler methods
    sample_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Most expensive, but pretty
    sample_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sample_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sample_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sample_desc.MipLODBias = 0.0f;
    sample_desc.MaxAnisotropy = 1;
    sample_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sample_desc.MinLOD = 0;
    sample_desc.MaxLOD = D3D11_FLOAT32_MAX;
    for (int i = 0; i < 4; i++)
        sample_desc.BorderColor[i] = 0;

    result = device_->CreateSamplerState(&sample_desc, &shader->sampler_state_);
    if(FAILED(result))
        return false;

    return true;
}

void RenderD3D11::SetModelBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)
{
    BufferResourceD3D11* vertex_buf = static_cast<BufferResourceD3D11*>(vertex_buffer);
    BufferResourceD3D11* index_buf  = static_cast<BufferResourceD3D11*>(index_buffer);

    unsigned int stride, offset;

    stride = sizeof(Vertex);
    offset = 0;

    // Vertex ACTIVE.....
    device_context_->IASetVertexBuffers(0, 1, &vertex_buf->p, &stride, &offset);

    // Indice.. ACTIVE!!!!
    device_context_->IASetIndexBuffer(index_buf->p, DXGI_FORMAT_R32_UINT, 0);

    // We do tris here
    device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}

void RenderD3D11::SetShader(ShaderResource* program, int index_count)
{
    ShaderResourceD3D11* shader = static_cast<ShaderResourceD3D11*>(program);
    // Make sure we shove them verts in so they fit rite
    device_context_->IASetInputLayout(shader->layout_);

    // our shader!!!!
    device_context_->VSSetShader(shader->vertex_shader_, nullptr, 0);
    device_context_->PSSetShader(shader->pixel_shader_, nullptr, 0);

    // make sure we render the pixels how i like
    device_context_->PSSetSamplers(0, 1, &shader->sampler_state_);

    // wow here it is. its so small. did you think itd be this small?
    device_context_->DrawIndexed(index_count, 0, 0);

    return;
}

bool RenderD3D11::SetShaderInputs(ShaderResource* program, TextureResource* texture,
                                  Matrix world_matrix, Matrix view_matrix, Matrix proj_matrix)
{
    ShaderResourceD3D11* shader = static_cast<ShaderResourceD3D11*>(program);
    TextureResourceD3D11* tex = static_cast<TextureResourceD3D11*>(texture);

    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    MatrixBuffer* data;
    unsigned int num_buffers;
    
    // Transpose matrices, required in DX11
    world_matrix = MatrixTranspose(world_matrix);
    view_matrix = MatrixTranspose(view_matrix);
    proj_matrix = MatrixTranspose(proj_matrix);

    // Lock buffer to gain write access
    result = device_context_->Map(shader->matrix_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    if (FAILED(result))
        return false;

    // Cast cbuffer to matrix
    data = (MatrixBuffer*)mapped_resource.pData;

    // Copy new matrix data
    data->world      = world_matrix;
    data->view       = view_matrix;
    data->projection = proj_matrix;

    // Unmap we done writing
    device_context_->Unmap(shader->matrix_buffer_, 0);

    // Why is this necessary
    num_buffers = 0;

    // Push the updated matrices
    device_context_->VSSetConstantBuffers(num_buffers, 1, &shader->matrix_buffer_);

    // Push the texture
    device_context_->PSSetShaderResources(0, 1, &tex->p);

    return true;
}

// Copy helpers
Matrix RenderD3D11::GetProjectionMatrix()
{
    return proj_matrix_;
}

Matrix RenderD3D11::GetWorldMatrix()
{
    return world_matrix_;
}

Matrix RenderD3D11::GetOrthoMatrix()
{
    return ortho_matrix_;
}

void RenderD3D11::GetVideoCardInfo(char* name, int& memory)
{
    strcpy_s(name, 128, video_card_desc_);
    memory = video_card_memory_;
    return;
}

TextureResource* RenderD3D11::LoadDDSFile(WCHAR* filename)
{
    HRESULT result;

    TextureResourceD3D11* texture = static_cast<TextureResourceD3D11*>(CreateTextureResource());

    result = DirectX::CreateDDSTextureFromFile(device_, filename, nullptr, &texture->p, 0);
    if(FAILED(result))
        return nullptr;

    return texture;
}

void RenderD3D11::OutputShaderErrorMessage(ID3D10Blob* error_message)
{
    char* compile_errors;
    unsigned long buffer_size;
    std::ofstream fout;

    compile_errors = (char*)(error_message->GetBufferPointer());
    buffer_size = error_message->GetBufferSize();

    fout.open("shader.log");

    for (unsigned int i = 0; i < buffer_size; i++)
        fout << compile_errors[i];

    fout.close();

    error_message->Release();
    error_message = nullptr;
    
    return;
}