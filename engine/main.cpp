#include "client.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Client* client;
    client = new Client;

    if (!client)
        return 0;

    if (client->Init())
        client->Run();

    client->Finish();
    delete client;

    return 0;
}