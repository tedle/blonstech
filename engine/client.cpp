#include "client.h"

CClient::CClient()
{
    m_Input = NULL;
    m_Graphics = NULL;
}

CClient::~CClient()
{
}

bool CClient::Init()
{
    int screenWidth, screenHeight;
    screenWidth = screenHeight = 0;

    // Open window and get w+h
    InitWindow(screenWidth, screenHeight);

    m_Input = new CInput;
    if(!m_Input)
        return false;

    // Do what the implementation needs to get started
    if(!m_Input->Init())
        return false;

    m_Graphics = new CGraphics;
    if(!m_Graphics)
        return false;

    // Figure out all that directy stuff
    if(!m_Graphics->Init(screenWidth, screenHeight, m_hwnd))
        return false;

    return true;
}

void CClient::Finish()
{
    if(m_Graphics)
    {
        m_Graphics->Finish();
        delete m_Graphics;
    }

    if(m_Input)
    {
        m_Input->Finish();
        delete m_Input;
    }

    FinishWindow();

    return;
}

void CClient::Run()
{
    MSG msg = {};
    bool quit;

    quit = false;
    while(!quit)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Is someone trying to X out
        if(msg.message == WM_QUIT)
            quit = true;
        else
            if(!Frame())
                quit = true;
    }
}

bool CClient::Frame()
{
    // Esc = exit
    if(!m_Input->Frame() || m_Input->IsKeyDown(VK_ESCAPE))
        return false;

    // Render scene
    if(!m_Graphics->Frame())
        return false;

    return true;
}

void CClient::InitWindow(int& screenWidth, int& screenHeight)
{
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;

    // TODO: Temporary until Direct & Raw input are setup
    ApplicationHandle = this;

    // Initialize some win32 stuff
    m_hinstance = GetModuleHandle(NULL);
    m_appName = L"blonstech 0.6.9b420";

    // Init window class
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_appName;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    // TODO: Do it in CGraphics later
    if(FULL_SCREEN)
    {
        screenWidth  = GetSystemMetrics(SM_CXSCREEN);
        screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // Init screen settings
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change screen settings
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        posX = posY = 0;
    }
    else
    {
        screenWidth = 800;
        screenHeight = 600;

        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // Finally make the window, fuck win32
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_appName, m_appName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER | WS_SYSMENU,
                            posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);



    /* FOR BORDERLESS WINDOW DO
        m_hwnd = CreateWindowEx(WS_EX_APPWINDOW|WS_EX_TOPMOST, m_appName, m_appName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
                            posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
        NOTE: YOU MUST MANUALLY DISABLE WS_EX_TOPMOST EVERYTIME YOU LOSE FOCUS, AND REENABLE WHENEVER FOCUS IS GAINED
        SetWindowPos(m_hwnd, HWND_TOPMOST, 500, 500, 800, 600, NULL); // COVERS TASKBAR
        SetWindowPos(m_hwnd, HWND_TOP, 500, 500, 800, 600, NULL); // DOESNT COVER TASKBAR
        */
            

    // Make it real
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    //ShowCursor(false);

    return;
}

void CClient::FinishWindow()
{
    ShowCursor(true);

    // TODO: Temporary until Direct & Raw input are setup
    if(FULL_SCREEN)
        ChangeDisplaySettings(NULL, 0);
    ApplicationHandle = NULL;

    DestroyWindow(m_hwnd);
    m_hwnd = NULL;

    UnregisterClass(m_appName, m_hinstance);
    m_hinstance = NULL;

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
        return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
    }
}

// TODO: Temporary until Direct & Raw input are setup
LRESULT CALLBACK CClient::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch(umsg)
    {
    case WM_KEYDOWN:
        OutputDebugString(L"HELLO!\n");
        m_Input->KeyDown((unsigned int)wparam);
        return 0;
    case WM_KEYUP:
        m_Input->KeyUp((unsigned int)wparam);
        return 0;
    default:
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
}