#ifndef __CLIENT_H__
#define __CLIENT_H__

// Strips less used APIs from inclusion
#define WIN32_LEAN_AND_MEAN

// Includes
#include <Windows.h>
// ---
#include "inputtemp.h"
#include "graphics.h"

// Client Class
// Controls entirety of program
class CClient{

public:
    // Public functions
    CClient();
    ~CClient();

    bool Init();
    void Finish();
    void Run();

    // TODO: Temporary until Direct & Raw input are setup
    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    // Private functions
    bool Frame();
    void InitWindow(int&, int&);
    void FinishWindow();

    // Private members
    LPCWSTR m_appName;
    HINSTANCE m_hinstance;
    HWND m_hwnd;

    CInput* m_Input;
    CGraphics* m_Graphics;
};

// Message hooking helpers
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// TODO: Temporary until Direct & Raw input are setup
static CClient* ApplicationHandle = NULL;

#endif