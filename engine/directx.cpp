#include "directx.h"

D3D::D3D()
{
    swapchain_ = NULL;
    device_ = NULL;
    device_context_ = NULL;
    render_target_view_ = NULL;
    depth_stencil_buffer_ = NULL;
    depth_stencil_state_ = NULL;
    depth_stencil_view_ = NULL;
    raster_state_ = NULL;
}

D3D::~D3D()
{
}

bool D3D::Init(int screen_width, int screen_height, bool vsync, HWND hwnd, bool fullscreen, float screen_depth, float screen_near)
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
    result = adapter_out->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, NULL);
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
    for(unsigned int i = 0; i < num_modes; i++)
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
    display_modes = NULL;

    adapter_out->Release();
    adapter_out = NULL;

    adapter->Release();
    adapter = NULL;

    factory->Release();
    factory = NULL;

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
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &feature_level, 1, D3D11_SDK_VERSION,
                                           &swapchain_desc, &swapchain_, &device_, NULL, &device_context_);
    if (FAILED(result))
        return false;

    result = swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
    if (FAILED(result))
        return false;

    // Init the render view
    result = device_->CreateRenderTargetView(back_buffer, NULL, &render_target_view_);
    if (FAILED(result))
        return false;

    back_buffer->Release();
    back_buffer = NULL;

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

    result = device_->CreateTexture2D(&depth_buffer_desc, NULL, &depth_stencil_buffer_);
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
    raster_desc.CullMode = D3D11_CULL_BACK; // Backface culling SO ftw...
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
    fov = (float)XM_PI / 4.0f;
    screen_aspect = (float)screen_width / (float)screen_height;

    XMStoreFloat4x4(&projection_matrix_, XMMatrixPerspectiveFovLH(fov, screen_aspect, screen_near, screen_depth));

    // World matrix (models->3D space) shouldnt this be in model class??
    // TODO: Make this part of model class, maybe separate world/view+proj matrix cbuffers, and update world matrix for each model rendered
    XMStoreFloat4x4(&world_matrix_, XMMatrixIdentity());

    // Ortho projection matrix (for 2d stuff)
    XMStoreFloat4x4(&ortho_matrix_, XMMatrixOrthographicLH((float)screen_width, (float)screen_height, screen_near, screen_depth));

    return true;
}

void D3D::Finish()
{
    // Always shut down a swapchain in windowed mode or problems happen
    if (swapchain_)
        swapchain_->SetFullscreenState(false, NULL);

    if (raster_state_)
    {
        raster_state_->Release();
        raster_state_ = NULL;
    }

    if (depth_stencil_view_)
    {
        depth_stencil_view_->Release();
        depth_stencil_view_ = NULL;
    }

    if (depth_stencil_state_)
    {
        depth_stencil_state_->Release();
        depth_stencil_state_ = NULL;
    }

    if (depth_stencil_buffer_)
    {
        depth_stencil_buffer_->Release();
        depth_stencil_buffer_ = NULL;
    }

    if (render_target_view_)
    {
        render_target_view_->Release();
        render_target_view_ = NULL;
    }

    if (device_context_)
    {
        device_context_->Release();
        device_context_ = NULL;
    }

    if (device_)
    {
        device_->Release();
        device_ = NULL;
    }

    if (swapchain_)
    {
        swapchain_->Release();
        swapchain_ = 0;
    }

    return;
}

// TODO: howabout we dont take these 4 dumb arguments
void D3D::BeginScene(float red, float green, float blue, float alpha)
{
    float colour[4];

    colour[0] = red;
    colour[1] = green;
    colour[2] = blue;
    colour[3] = alpha;

    device_context_->ClearRenderTargetView(render_target_view_, colour);

    device_context_->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH, 1.0f, 0);

    return;
}

void D3D::EndScene()
{
    swapchain_->Present(vsync_, 0);

    return;
}

ID3D11Device* D3D::GetDevice()
{
    return device_;
}

ID3D11DeviceContext* D3D::GetDeviceContext()
{
    return device_context_;
}

// Copy helpers
XMFLOAT4X4 D3D::GetProjectionMatrix()
{
    return projection_matrix_;
}

XMFLOAT4X4 D3D::GetWorldMatrix()
{
    return world_matrix_;
}

XMFLOAT4X4 D3D::GetOrthoMatrix()
{
    return ortho_matrix_;
}

void D3D::GetVideoCardInfo(char* name, int& memory)
{
    strcpy_s(name, 128, video_card_desc_);
    memory = video_card_memory_;
    return;
}