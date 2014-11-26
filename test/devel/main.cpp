#include <include/blons.h>
#include <include/temphelpers.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
    blons::Client* client = new blons::Client;
    auto info = client->screen_info();
    auto graphics = std::unique_ptr<blons::Graphics>(new blons::Graphics(info.width, info.height, info.hwnd));
    auto gui = graphics->gui();
    std::vector<std::unique_ptr<blons::Model>> models;

    // Model 1
    models.push_back(graphics->MakeModel("../../notes/teapot_highpoly.bms"));
    models[0]->set_pos(0.0, 0.0, 20.0);

    // Model 2
    models.push_back(graphics->MakeModel("../../notes/cube.bms"));
    models[1]->set_pos(10.0, 0.0, 20.0);
    //models = blons::load_codmap("../../notes/bms_test", std::move(models), graphics.get());

    auto sprite = graphics->MakeSprite("../../notes/me.dds");
    sprite->set_pos(0, 0, 32, 32);

    gui->MakeWindow("test", 20, 80, 400, 200, "Friendly window");
    gui->MakeWindow("yoyo", 450, 250, 300, 300, "Amicable window");
    gui->window("test")->MakeLabel(10, 70, "HAello! blonsUI in action!");
    gui->window("test")->MakeButton(10, 100, 120, 60, "Button!")->set_callback([](){blons::g_log->Debug("hi hi!\n");});

    auto start = GetTickCount64() + 5000;
    bool quit = false;
    while (!quit)
    {
        /*if (GetTickCount64() > start + 100 && models.size() > 0)
        {
            models.pop_back();
            start = GetTickCount64();
            sprite.reset();
        }*/
        quit = client->Frame();
        blons::temp::FPS();
        bool gui_used_input = gui->Update(*client->input());
        // TODO: THIS IS TEMP DELETE LATER
        // Handles mouselook and wasd movement
        if (!gui_used_input)
        {
            blons::temp::noclip(client->input(), graphics->camera());
        }
        graphics->Render();
    }
    graphics.reset();

    delete client;

    return 0;
}
