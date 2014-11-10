#include "os/client.h"

// Local Includes
#include "debug/loggeride.h"
#include "temphelpers.h"

namespace blons
{
std::unique_ptr<LoggerAPI> g_log = nullptr;

Client::Client()
{
    g_log = nullptr;

    input_ = nullptr;
    graphics_ = nullptr;

    // Initialize logger
    g_log = std::unique_ptr<LoggerAPI>(new LoggerIDE(LoggerAPI::Level::DEBUG));

    int screen_width, screen_height;
    screen_width = screen_height = 0;

    // Open window and get w+h
    InitWindow(&screen_width, &screen_height);

    input_ = std::unique_ptr<Input>(new Input);
    if (input_ == nullptr)
    {
        throw "Failed input initilization";
    }

    graphics_ = std::unique_ptr<Graphics>(new Graphics(screen_width, screen_height, hwnd_));
    if (graphics_ == nullptr)
    {
        throw "Failed graphics initialization";
    }
}

Client::~Client()
{
    ShowCursor(true);

    // TODO: Temporary until Direct & Raw input are setup
    if (kRenderMode == kRenderModeFullscreen)
    {
        ChangeDisplaySettings(nullptr, 0);
    }
    g_application_handle = nullptr;

    DestroyWindow(hwnd_);
    hwnd_ = nullptr;

    UnregisterClass(app_name_, hinstance_);
    hinstance_ = nullptr;

    return;
}

void Client::Run()
{
    MSG msg = {};
    bool quit;

    quit = false;
    while(!quit)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Is someone trying to X out
        if (msg.message == WM_QUIT)
        {
            quit = true;
        }
        else
        {
            if (!Frame())
            {
                quit = true;
            }
        }
    }
}

bool Client::Frame()
{
    FPS();
    // Esc = exit
    if (!input_->Frame() || input_->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    // TODO: THIS IS TEMP DELETE LATER
    // Handles mouselook and wasd movement
    noclip(input_.get(), graphics_->camera());
    //move_camera_around_origin(1.0f, graphics_->camera());
    /*if (input_->IsKeyDown('A'))
    {
        move_camera_around_origin(-1.0f, graphics_->camera());
    }
    if (input_->IsKeyDown('D'))
    {
        move_camera_around_origin(1.0f, graphics_->camera());
    }*/
    // END TEMP

    // Render scene
    if (!graphics_->Frame())
    {
        return false;
    }

    return true;
}

void Client::InitWindow(int* screen_width, int* screen_height)
{
    WNDCLASSEX wc = {};
    DEVMODE screen_settings;
    int pos_x, pos_y;
    int r_width, r_height;
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    // TODO: Temporary until Direct & Raw input are setup
    g_application_handle = this;

    // Initialize some win32 stuff
    hinstance_ = GetModuleHandle(nullptr);
    app_name_ = L"blonstech 0.6.9b420";

    // Init window class
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinstance_;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = app_name_;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    // TODO: Do it in Graphics later
    if (kRenderMode == kRenderModeFullscreen)
    {
        r_width  = GetSystemMetrics(SM_CXSCREEN);
        r_height = GetSystemMetrics(SM_CYSCREEN);

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
        r_width  = 800;
        r_height = 600;

        pos_x = (GetSystemMetrics(SM_CXSCREEN) - r_width)  / 2;
        pos_y = (GetSystemMetrics(SM_CYSCREEN) - r_height) / 2;

        style |= WS_BORDER | WS_SYSMENU;
    }
    else if (kRenderMode == kRenderModeBorderlessWindow)
    {
        r_width  = GetSystemMetrics(SM_CXSCREEN);
        r_height = GetSystemMetrics(SM_CYSCREEN);

        pos_x = pos_y = 0;

        style |= WS_POPUP;
    }

    // Finally make the window, fuck win32
    hwnd_ = CreateWindowEx(WS_EX_APPWINDOW, app_name_, app_name_, style,
                            pos_x, pos_y, r_width, r_height, nullptr, nullptr, hinstance_, nullptr);

    // Make it real
    ShowWindow(hwnd_, SW_SHOW);
    SetForegroundWindow(hwnd_);
    SetFocus(hwnd_);

    // Poll for inner window dimensions (r_width/height have window border tacked on)
    RECT rect;
    GetClientRect(hwnd_, &rect); 
    (*screen_width)  = rect.right;
    (*screen_height) = rect.bottom;

    //ShowCursor(false);

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
        input_->KeyDown((unsigned int)wparam);
        return 0;
    case WM_KEYUP:
        input_->KeyUp((unsigned int)wparam);
        return 0;
    case WM_MOUSEMOVE:
        if (wparam & MK_LBUTTON)
        {
            input_->MouseDown(0);
        }
        else
        {
            input_->MouseUp(0);
        }
        if (wparam & MK_RBUTTON)
        {
            input_->MouseDown(1);
        }
        else
        {
            input_->MouseUp(1);
        }
        input_->MouseMove(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        /* g_log->Debug("pos(%i,%i), delta(%i,%i), down(%i, %i)\n",
                     input_->MouseX(), input_->MouseY(), input_->MouseDeltaX(), input_->MouseDeltaY(),
                     input_->IsMouseDown(0), input_->IsMouseDown(1)); */
        return 0;
    default:
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
}
} // namespace blons
