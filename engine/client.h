#ifndef BLONSTECH_CLIENT_H_
#define BLONSTECH_CLIENT_H_

// Strips less used APIs from inclusion
#define WIN32_LEAN_AND_MEAN

// Includes
#include <memory>
#include <Windows.h>
// Local Includes
#include "inputtemp.h"
#include "graphics.h"

// Client Class
// Controls entirety of program
class Client{

public:
    // Public functions
    Client();
    ~Client();

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
    LPCWSTR app_name_;
    HINSTANCE hinstance_;
    HWND hwnd_;

    std::unique_ptr<Input> input_;
    std::unique_ptr<Graphics> graphics_;
};

// Message hooking helpers
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// TODO: Temporary until Direct & Raw input are setup
static Client* g_application_handle = nullptr;

#endif