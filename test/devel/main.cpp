#include <include/blons.h>
#include <include/temphelpers.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
    blons::Client* client = new blons::Client;
    auto info = client->screen_info();

    if (!client)
        return 0;
    std::vector<std::unique_ptr<blons::Model>> models;

    auto graphics = std::unique_ptr<blons::Graphics>(new blons::Graphics(info.width, info.height, info.hwnd));
    // Model 1
    models.push_back(graphics->CreateModel("../../notes/teapot_highpoly.bms"));
    if (models[0] == nullptr)
    {
        blons::g_log->Fatal("FATAL: Teapot initialization procedures were unsuccessful\n");
        return 1;
    }
    models[0]->set_pos(0.0, 0.0, 20.0);

    // Model 2
    models.push_back(graphics->CreateModel("../../notes/cube.bms"));
    if (models[1] == nullptr)
    {
        blons::g_log->Fatal("no cube :(\n");
        return 1;
    }
    models[1]->set_pos(10.0, 0.0, 20.0);
    //models = blons::load_codmap("../../notes/bms_test", std::move(models), graphics.get());

    auto start = GetTickCount64() + 5000;
    bool quit = false;
    while (!quit)
    {
        /*if (GetTickCount64() > start + 100 && models.size() > 0)
        {
            models.pop_back();
            start = GetTickCount64();
        }*/
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
