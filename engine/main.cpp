#include "client.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    CClient* Client;
    Client = new CClient;

    if(!Client)
        return 0;

    if(Client->Init())
        Client->Run();

    Client->Finish();
    delete Client;

    return 0;
}