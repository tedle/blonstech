#include "client.h"

Client::Client()
{
    input_ = NULL;
    graphics_ = NULL;
}

Client::~Client()
{
}

bool Client::Init()
{
    int screen_width, screen_height;
    screen_width = screen_height = 0;

    // Open window and get w+h
    InitWindow(screen_width, screen_height);

    input_ = new Input;
    if (!input_)
        return false;

    // Do what the implementation needs to get started
    if (!input_->Init())
        return false;

    graphics_ = new Graphics;
    if (!graphics_)
        return false;

    // Figure out all that directy stuff
    if (!graphics_->Init(screen_width, screen_height, hwnd_))
        return false;

    return true;
}

void Client::Finish()
{
    if (graphics_)
    {
        graphics_->Finish();
        delete graphics_;
    }

    if (input_)
    {
        input_->Finish();
        delete input_;
    }

    FinishWindow();

    return;
}

void Client::Run()
{
    MSG msg = {};
    bool quit;

    quit = false;
    while(!quit)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Is someone trying to X out
        if (msg.message == WM_QUIT)
            quit = true;
        else
            if (!Frame())
                quit = true;
    }
}


// TODO: DELETE THIS TURD FUNCTION
DWORD last_time = 0;
int fps_count = 0;
void FPS()
{
    DWORD st = GetTickCount();

    if (st > last_time+1000)
    {
        char msg[64];
        sprintf(msg, "FPS: %i\n", fps_count);
        OutputDebugStringA(msg);
        last_time = st;
        fps_count = 0;
    }
    else
        fps_count++;
}
// TODO: ^^^GET RID OF THAT^^^

bool Client::Frame()
{
    FPS();
    // Esc = exit
    if (!input_->Frame() || input_->IsKeyDown(VK_ESCAPE))
        return false;

    // Render scene
    if (!graphics_->Frame())
        return false;

    return true;
}

void Client::InitWindow(int& screen_width, int& screen_height)
{
    WNDCLASSEX wc;
    DEVMODE screen_settings;
    int pos_x, pos_y;
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    // TODO: Temporary until Direct & Raw input are setup
    g_application_handle = this;

    // Initialize some win32 stuff
    hinstance_ = GetModuleHandle(NULL);
    app_name_ = L"blonstech 0.6.9b420";

    // Init window class
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinstance_;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = app_name_;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    // TODO: Do it in Graphics later
    if (kRenderMode == kRenderModeFullscreen)
    {
        screen_width  = GetSystemMetrics(SM_CXSCREEN);
        screen_height = GetSystemMetrics(SM_CYSCREEN);

        // Init screen settings
        memset(&screen_settings, 0, sizeof(screen_settings));
        screen_settings.dmSize       = sizeof(screen_settings);
        screen_settings.dmPelsWidth  = (unsigned long)screen_width;
        screen_settings.dmPelsHeight = (unsigned long)screen_height;
        screen_settings.dmBitsPerPel = 32;
        screen_settings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change screen settings
        ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);

        pos_x = pos_y = 0;
    }
    else if (kRenderMode == kRenderModeWindow)
    {
        screen_width = 800;
        screen_height = 600;

        pos_x = (GetSystemMetrics(SM_CXSCREEN) - screen_width) / 2;
        pos_y = (GetSystemMetrics(SM_CYSCREEN) - screen_height) / 2;

        style |= WS_BORDER | WS_SYSMENU;
    }
    else if (kRenderMode == kRenderModeBorderlessWindow)
    {
        screen_width  = GetSystemMetrics(SM_CXSCREEN);
        screen_height = GetSystemMetrics(SM_CYSCREEN);

        pos_x = pos_y = 0;

        style |= WS_POPUP;
    }

    // Finally make the window, fuck win32
    hwnd_ = CreateWindowEx(WS_EX_APPWINDOW, app_name_, app_name_, style,
                            pos_x, pos_y, screen_width, screen_height, NULL, NULL, hinstance_, NULL);

    // Make it real
    ShowWindow(hwnd_, SW_SHOW);
    SetForegroundWindow(hwnd_);
    SetFocus(hwnd_);

    //ShowCursor(false);

    return;
}

void Client::FinishWindow()
{
    ShowCursor(true);

    // TODO: Temporary until Direct & Raw input are setup
    if (kRenderMode == kRenderModeFullscreen)
        ChangeDisplaySettings(NULL, 0);
    g_application_handle = NULL;

    DestroyWindow(hwnd_);
    hwnd_ = NULL;

    UnregisterClass(app_name_, hinstance_);
    hinstance_ = NULL;

    return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch(umsg)
    {
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    default:
        // TODO: Temporary until Direct & Raw input are setup
        return g_application_handle->MessageHandler(hwnd, umsg, wparam, lparam);
    }
}

// TODO: Temporary until Direct & Raw input are setup
LRESULT CALLBACK Client::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch(umsg)
    {
    case WM_KEYDOWN:
        OutputDebugString(L"HELLO!\n");
        input_->KeyDown((unsigned int)wparam);
        return 0;
    case WM_KEYUP:
        input_->KeyUp((unsigned int)wparam);
        return 0;
    default:
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
}