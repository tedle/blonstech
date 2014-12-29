////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/system/client.h>

// Includes
#include <windowsx.h>
// Public Includes
#include <blons/debug.h>
#include <blons/graphics/graphics.h>


namespace blons
{
// TODO: Temporary until Direct & Raw input are setup
static Client* g_application_handle = nullptr;
// Message hooking helpers
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
Client::Client()
{
    input_ = nullptr;

    // Initialize logger
    log::SetOutputLevel(log::DEBUG);
    // TODO: Temporary callback function, make something more extensible later
    log::SetPrintCallback([](const std::string& s){ OutputDebugStringA(s.c_str()); });
    console::RegisterPrintCallback([](const std::string& s){ log::Debug("[console] %s", s.c_str()); });

    units::pixel screen_width, screen_height;
    screen_width = screen_height = 0;

    // Open window and get w+h
    InitWindow(&screen_width, &screen_height);
    screen_info_.width = screen_width;
    screen_info_.height = screen_height;

    input_.reset(new Input);
    if (input_ == nullptr)
    {
        throw "Failed input initilization";
    }
}

Client::~Client()
{
    ShowCursor(true);

    // TODO: Temporary until Direct & Raw input are setup
    if (kRenderMode == RenderMode::FULLSCREEN)
    {
        ChangeDisplaySettings(nullptr, 0);
    }
    g_application_handle = nullptr;

    DestroyWindow(screen_info_.hwnd);
    screen_info_.hwnd = nullptr;

    UnregisterClass(app_name_, hinstance_);
    hinstance_ = nullptr;

    return;
}

bool Client::Frame()
{
    MSG msg = {};

    bool quit = false;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Is someone trying to X out
        if (msg.message == WM_QUIT)
        {
            quit = true;
        }
    }

    input_->Frame();
    // Esc = exit
    if (input_->IsKeyDown(Input::ESCAPE))
    {
        return true;
    }

    return quit;
}

Client::Info Client::screen_info() const
{
    return screen_info_;
}

void Client::InitWindow(units::pixel* screen_width, units::pixel* screen_height)
{
    WNDCLASSEX wc = {};
    DEVMODE screen_settings;
    units::pixel pos_x, pos_y;
    units::pixel r_width, r_height;
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
    if (kRenderMode == RenderMode::FULLSCREEN)
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
    else if (kRenderMode == RenderMode::WINDOW)
    {
        r_width  = 800;
        r_height = 600;

        pos_x = (GetSystemMetrics(SM_CXSCREEN) - r_width)  / 2;
        pos_y = (GetSystemMetrics(SM_CYSCREEN) - r_height) / 2;

        style |= WS_BORDER | WS_SYSMENU;
    }
    else if (kRenderMode == RenderMode::BORDERLESS_WINDOW)
    {
        r_width  = GetSystemMetrics(SM_CXSCREEN);
        r_height = GetSystemMetrics(SM_CYSCREEN);

        pos_x = pos_y = 0;

        style |= WS_POPUP;
    }

    // Finally make the window, fuck win32
    screen_info_.hwnd = CreateWindowEx(WS_EX_APPWINDOW, app_name_, app_name_, style,
                                       pos_x, pos_y, r_width, r_height, nullptr, nullptr, hinstance_, nullptr);

    // Make it real
    ShowWindow(screen_info_.hwnd, SW_SHOW);
    SetForegroundWindow(screen_info_.hwnd);
    SetFocus(screen_info_.hwnd);

    // Poll for inner window dimensions (r_width/height have window border tacked on)
    RECT rect;
    GetClientRect(screen_info_.hwnd, &rect); 
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
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        bool is_down = ((lparam & (1 << 31)) == 0);
        Input::KeyCode key_code = Input::BAD;
        // Translate key_code
        {
            if (wparam >= '0' && wparam <= '9')
            {
                key_code = static_cast<Input::KeyCode>(wparam - '0' + Input::CHAR_0);
            }
            else if (wparam >= 'A' && wparam <= 'Z')
            {
                key_code = static_cast<Input::KeyCode>(wparam - 'A' + Input::CHAR_A);
            }
            else
            {
                switch (wparam)
                {
                case VK_OEM_4:
                    key_code = Input::SYMBOL_LEFT_BRACKET;
                    break;
                case VK_OEM_6:
                    key_code = Input::SYMBOL_RIGHT_BRACKET;
                    break;
                case VK_OEM_3:
                    key_code = Input::SYMBOL_GRAVE_ACCENT;
                    break;
                case VK_OEM_5:
                    key_code = Input::SYMBOL_BACKSLASH;
                    break;
                case VK_OEM_1:
                    key_code = Input::SYMBOL_SEMICOLON;
                    break;
                case VK_OEM_7:
                    key_code = Input::SYMBOL_QUOTE;
                    break;
                case VK_OEM_COMMA:
                    key_code = Input::SYMBOL_COMMA;
                    break;
                case VK_OEM_PERIOD:
                    key_code = Input::SYMBOL_PERIOD;
                    break;
                case VK_OEM_2:
                    key_code = Input::SYMBOL_SLASH;
                    break;
                case VK_OEM_MINUS:
                    key_code = Input::SYMBOL_MINUS;
                    break;
                case VK_OEM_PLUS:
                    key_code = Input::SYMBOL_EQUALS;
                    break;
                case VK_SPACE:
                    key_code = Input::SPACE;
                    break;
                case VK_SHIFT:
                    key_code = Input::SHIFT;
                    break;
                case VK_CONTROL:
                    key_code = Input::CONTROL;
                    break;
                case VK_MENU:
                    key_code = Input::ALT;
                    break;
                case VK_BACK:
                    key_code = Input::BACKSPACE;
                    break;
                case VK_DELETE:
                    key_code = Input::DEL;
                    break;
                case VK_ESCAPE:
                    key_code = Input::ESCAPE;
                    break;
                case VK_RETURN:
                    key_code = Input::RETURN;
                    break;
                case VK_TAB:
                    key_code = Input::TAB;
                    break;
                case VK_PRIOR:
                    key_code = Input::PG_UP;
                    break;
                case VK_NEXT:
                    key_code = Input::PG_DOWN;
                    break;
                case VK_LEFT:
                    key_code = Input::LEFT;
                    break;
                case VK_RIGHT:
                    key_code = Input::RIGHT;
                    break;
                case VK_DOWN:
                    key_code = Input::DOWN;
                    break;
                case VK_UP:
                    key_code = Input::UP;
                    break;
                default:
                    key_code = Input::BAD;
                    break;
                }
            }
        }
        if (is_down)
        {
            input_->KeyDown(key_code);
        }
        else
        {
            input_->KeyUp(key_code);
        }
        return 0;
    }
    // I know Win32 is bad, but c'mon...
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
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
        if (wparam & MK_MBUTTON)
        {
            input_->MouseDown(2);
        }
        else
        {
            input_->MouseUp(2);
        }
        if (wparam & MK_XBUTTON1)
        {
            input_->MouseDown(3);
        }
        else
        {
            input_->MouseUp(3);
        }
        if (wparam & MK_XBUTTON2)
        {
            input_->MouseDown(4);
        }
        else
        {
            input_->MouseUp(4);
        }
        input_->MouseMove(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        return 0;
    case WM_MOUSEWHEEL:
        input_->MouseScroll(GET_WHEEL_DELTA_WPARAM(wparam) / 120);
        return 0;
    default:
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
}

Input* Client::input() const
{
    return input_.get();
}
} // namespace blons
