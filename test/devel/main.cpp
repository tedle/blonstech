#include <engine/client.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
    blons::Client* client = new blons::Client;

    if (!client)
        return 0;

    if (client->Init())
        client->Run();

    client->Finish();
    delete client;

    return 0;
}
