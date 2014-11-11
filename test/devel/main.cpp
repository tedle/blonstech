#include <include/blons.h>
#include <include/temphelpers.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
    blons::Client* client = new blons::Client;
    auto info = client->screen_info();

    if (!client)
        return 0;

    auto graphics = std::unique_ptr<blons::Graphics>(new blons::Graphics(info.width, info.height, info.hwnd));
    bool quit = false;
    while (!quit)
    {
        quit = client->Frame();
        blons::FPS();
        // TODO: THIS IS TEMP DELETE LATER
        // Handles mouselook and wasd movement
        blons::noclip(client->input(), graphics->camera());
        graphics->Render();
    }

    delete client;

    return 0;
}
