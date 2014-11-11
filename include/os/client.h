#ifndef BLONSTECH_CLIENT_H_
#define BLONSTECH_CLIENT_H_

// Strips less used APIs from inclusion
#define WIN32_LEAN_AND_MEAN

// Includes
#include <memory>
#include <Windows.h>
// Local Includes
#include "input/inputtemp.h"

namespace blons
{
// Client Class
// Controls entirety of program
class Client{

public:
    struct Info
    {
        int width, height;
        HWND hwnd;
    };

    // Public functions
    Client();
    ~Client();

    bool Frame();

    Info screen_info();

    // TODO: Temporary until Direct & Raw input are setup
    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
    Input* input();

private:
    // Private functions
    void InitWindow(int* width, int* height);

    // Private members
    LPCWSTR app_name_;
    HINSTANCE hinstance_;
    Info screen_info_;

    std::unique_ptr<Input> input_;
};

// Message hooking helpers
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// TODO: Temporary until Direct & Raw input are setup
static Client* g_application_handle = nullptr;
} // namespace blons

#endif