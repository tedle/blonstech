#include "directx.h"

CD3D::CD3D()
{
    m_swapChain = NULL;
    m_device = NULL;
    m_deviceContext = NULL;
    m_renderTargetView = NULL;
    m_depthStencilBuffer = NULL;
    m_depthStencilState = NULL;
    m_depthStencilView = NULL;
    m_rasterState = NULL;
}

CD3D::~CD3D()
{
}

bool CD3D::Init(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
    HRESULT result;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes, numerator, denominator, stringLength;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBuffer;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_RASTERIZER_DESC rasterDesc;
    D3D11_VIEWPORT viewport;
    float fov, screenAspect;

    m_vsync = vsync;

    // Get a interface
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if(FAILED(result))
        return false;

    // Get a video card
    result = factory->EnumAdapters(0, &adapter);
    if(FAILED(result))
        return false;

    // Sort thru viddy cards
    result = adapter->EnumOutputs(0, &adapterOutput);
    if(FAILED(result))
        return false;

    // Get only the best of the display modess
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if(FAILED(result))
        return false;

    // Allocate a sucker to find what modes we g0t
    displayModeList = new DXGI_MODE_DESC[numModes];
    if(!displayModeList)
        return false;

    // Stuff that sucker up!!!
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if(FAILED(result))
        return false;

    // Now to actually find one compatible w/ what i got
    for(unsigned int i = 0; i < numModes; i++)
    {
        if(displayModeList[i].Width  == (unsigned int)screenWidth
        && displayModeList[i].Height == (unsigned int)screenHeight)
        {
            numerator = displayModeList[i].RefreshRate.Numerator;
            denominator = displayModeList[i].RefreshRate.Denominator;
        }
    }

    // Tell me bout vid card
    result = adapter->GetDesc(&adapterDesc);
    if(FAILED(result))
        return false;

    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    if(wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128))
        return false;

    // Clean up some mamorie
    delete [] displayModeList;
    displayModeList = NULL;

    adapterOutput->Release();
    adapterOutput = NULL;

    adapter->Release();
    adapter = NULL;

    factory->Release();
    factory = NULL;

    // Init swap chain
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Triple buffering blows
    swapChainDesc.BufferCount = 1;

    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    swapChainDesc.BufferDesc.RefreshRate.Numerator   = m_vsync ? numerator   : 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = m_vsync ? denominator : 1;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    swapChainDesc.OutputWindow = hwnd;

    // No multisampling ...
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.Windowed = !fullscreen;

    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    swapChainDesc.Flags = 0;

    // 11.1 can fuck right off
    featureLevel = D3D_FEATURE_LEVEL_11_0;

    // FINALLY initialize directx holey shit
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION,
                                           &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if(FAILED(result))
        return false;

    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if(FAILED(result))
        return false;

    // Init the render view
    result = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
    if(FAILED(result))
        return false;

    backBuffer->Release();
    backBuffer = NULL;

    // Init depth + stencil buffer
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if(FAILED(result))
        return false;

    // How thhe stencil work
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if(FAILED(result))
        return false;

    // Finally plug it in
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

    // AND teach dirx how to use depth buffer
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if(FAILED(result))
        return false;

    // attach 2 pipeline
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    // Setup personal rasterizer state for tinkering mayb...
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK; // Backface culling SO ftw...
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
    if(FAILED(result))
        return false;

    // and apply!
    m_deviceContext->RSSetState(m_rasterState);

    // Time to get to work on the viewport!!!! (oof exciting)
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    m_deviceContext->RSSetViewports(1, &viewport);

    // Projection matrix (3D space->2D screen)
    fov = (float)XM_PI / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(fov, screenAspect, screenNear, screenDepth));

    // World matrix (models->3D space)
    XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());

    // Ortho projection matrix (for 2d stuff)
    XMStoreFloat4x4(&m_orthoMatrix, XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth));

    return true;
}

void CD3D::Finish()
{
    // Always shut down a swapchain in windowed mode or problems happen
    if(m_swapChain)
        m_swapChain->SetFullscreenState(false, NULL);

    if(m_rasterState)
    {
        m_rasterState->Release();
        m_rasterState = NULL;
    }

    if(m_depthStencilView)
    {
        m_depthStencilView->Release();
        m_depthStencilView = NULL;
    }

    if(m_depthStencilState)
    {
        m_depthStencilState->Release();
        m_depthStencilState = NULL;
    }

    if(m_depthStencilBuffer)
    {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = NULL;
    }

    if(m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = NULL;
    }

    if(m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = NULL;
    }

    if(m_device)
    {
        m_device->Release();
        m_device = NULL;
    }

    if(m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = 0;
    }

    return;
}

void CD3D::BeginScene(float red, float green, float blue, float alpha)
{
    float colour[4];

    colour[0] = red;
    colour[1] = green;
    colour[2] = blue;
    colour[3] = alpha;

    m_deviceContext->ClearRenderTargetView(m_renderTargetView, colour);

    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    return;
}

void CD3D::EndScene()
{
    m_swapChain->Present(m_vsync, 0);

    return;
}

ID3D11Device* CD3D::GetDevice()
{
    return m_device;
}

ID3D11DeviceContext* CD3D::GetDeviceContext()
{
    return m_deviceContext;
}

// Copy helpers
XMFLOAT4X4 CD3D::GetProjectionMatrix()
{
    return m_projectionMatrix;
}

XMFLOAT4X4 CD3D::GetWorldMatrix()
{
    return m_worldMatrix;
}

XMFLOAT4X4 CD3D::GetOrthoMatrix()
{
    return m_orthoMatrix;
}

void CD3D::GetVideoCardInfo(char* name, int& memory)
{
    strcpy_s(name, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
    return;
}