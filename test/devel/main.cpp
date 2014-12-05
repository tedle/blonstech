#include <include/blons.h>
#include <include/temphelpers.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
    auto client = std::unique_ptr<blons::Client>(new blons::Client);
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

    // Sprite 1
    auto sprite = graphics->MakeSprite("../../notes/me.dds");
    sprite->set_pos(0, 0, 32, 32);

    // GUI testing
    gui->MakeWindow("yoyo", 450, 250, 300, 300, "Amicable window");
    gui->MakeWindow("test", 20, 80, 400, 200, "Friendly window");

    auto textarea = gui->window("yoyo")->MakeTextarea(10, 40, 280, 205);
    for (int i = 0; i < 20; i++)
    {
        textarea->AddLine("Simple test line that isn't too short,\nmaybe even a little long, thanks for the friendly test!");
    }

    textarea->AddLine("HAello! blonsUI in action!");

    auto textbox = gui->window("test")->MakeTextbox(135, 150, 255, 40);
    auto print = [textbox]()
    {
        blons::g_log->Debug("%s\n", textbox->text().c_str());
        textbox->set_text("");
    };
    auto textareatest = [textarea, textbox]()
    {
        textarea->Clear();
        textarea->AddLine(textbox->text());
    };
    textbox->set_callback(textareatest);
    gui->window("test")->MakeButton(10, 150, 120, 40, "Print!")->set_callback(print);

    // Animation testing
    blons::Animation::Callback cbd = [](float d){ blons::g_log->Debug("%.4f\n", d); };
    blons::Animation::Callback cbx = [&](float d){ gui->window("test")->set_pos(d * 600 - 500, gui->window("test")->pos().y); };
    blons::Animation::Callback cby = [&](float d){ gui->window("test")->set_pos(gui->window("test")->pos().x, 362 - d * 300); };
    blons::Animation animatex(800, cbx, blons::Animation::QUINT_OUT);
    //blons::Animation animatey(500, cby, blons::Animation::QUINT_OUT);

    gui->window("yoyo")->MakeButton(10, 250, 280, 40, "New window!")->set_callback([&animatex](){ animatex.Reset(); });

    bool quit = false;
    while (!quit)
    {
        quit = client->Frame();
        blons::temp::FPS();
        bool gui_used_input = gui->Update(*client->input());
        // TODO: THIS IS TEMP DELETE LATER
        // Handles mouselook and wasd movement
        if (!gui_used_input)
        {
            blons::temp::noclip(client->input(), graphics->camera());
        }
        animatex.Update();
        //animatey.Update();
        graphics->Render();
    }

    return 0;
}
