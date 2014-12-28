////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/blons.h>
#include <blons/temphelpers.h>

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
    models = blons::temp::load_codmap("../../notes/bms_test", std::move(models), graphics.get());

    // Sprite 1
    auto sprite = graphics->MakeSprite("../../notes/me.dds");
    sprite->set_pos(0, 0, 32, 32);

    // GUI testing
    gui->MakeWindow("yoyo", 450, 250, 300, 300, "Amicable window");
    gui->MakeWindow("test", 20, 80, 400, 200, "Friendly window");

    auto textarea = gui->window("yoyo")->MakeTextarea(10, 40, 280, 205);
    for (int i = 0; i < 5; i++)
    {
        textarea->AddLine("Simple test line that isn't too short,\nmaybe even a little long, thanks for the friendly test!");
    }

    textarea->AddLine("HAello! blonsUI in action!");

    auto textbox = gui->window("test")->MakeTextbox(135, 150, 255, 40);
    auto print = [textbox]()
    {
        blons::log::Debug("%s\n", textbox->text().c_str());
        textbox->set_text("");
    };
    auto textareaclear = [textarea]()
    {
        textarea->Clear();
    };
    auto textareaprint = [textarea](blons::gui::Textbox* textbox)
    {
        for (int i = 0; i < 1; i++)
        {
            textarea->AddLine(textbox->text());
        }
        textbox->set_text("");
    };
    textbox->set_callback(textareaprint);
    gui->window("test")->MakeButton(10, 150, 120, 40, "Print!")->set_callback(print);
    gui->window("test")->MakeButton(10, 105, 380, 40, "Clear!")->set_callback(textareaclear);

    // Animation testing
    blons::Animation::Callback cbd = [](float d){ blons::log::Debug("%.4f\n", d); };
    blons::Animation::Callback cbx = [&](float d){ gui->window("test")->set_pos(d * 600 - 500, gui->window("test")->pos().y); };
    blons::Animation::Callback cby = [&](float d){ gui->window("test")->set_pos(gui->window("test")->pos().x, 362 - d * 300); };
    blons::Animation animatex(800, cbx, blons::Animation::QUINT_OUT);
    //blons::Animation animatey(500, cby, blons::Animation::QUINT_OUT);

    gui->window("yoyo")->MakeButton(10, 250, 280, 40, "New window!")->set_callback([&animatex](){ animatex.Reset(); });

    blons::console::out("Testing a string!\n");

    std::function<void(int)> test_func = [](int i){ blons::console::out("%i gamers in the house!\n", i); };
    std::function<void(const char*)> test_func_s = [](const char* i){ blons::console::out("%s stringers in the house!\n", i); };
    std::function<void(int, int)> test_func_ii = [](int i, int j){blons::console::out("%i, %i double inters in the house!\n", i, j); };
    blons::console::RegisterFunction("testo", test_func);
    blons::console::RegisterFunction("testo", test_func_s);
    blons::console::RegisterFunction("testo", test_func_ii);

    blons::console::RegisterVariable("a", 5);
    blons::console::RegisterVariable("b", 3.14f);
    blons::console::RegisterVariable("c", "heyo heyo heyo heyo");

    blons::console::set_var("a", 10);

    auto v_a = blons::console::var<int>("a");
    auto v_b = blons::console::var<float>("b");
    auto v_c = blons::console::var<std::string>("c");

    bool vid_restart = false;
    //blons::console::RegisterFunction("vid_restart", [&](){ vid_restart = true; });
    blons::console::RegisterFunction("vid_restart", [&](){ graphics->Reload(info.width, info.height, info.hwnd); });

    bool quit = false;
    while (!quit)
    {
        if (vid_restart)
        {
            vid_restart = false;
            graphics->Reload(info.width, info.height, info.hwnd);
        }
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
