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

void InitTestUI(blons::gui::Manager* gui);
void InitTestConsole(blons::Graphics* graphics, blons::Client::Info info);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
    auto client = std::make_unique<blons::Client>();
    auto info = client->screen_info();
    auto graphics = std::make_unique<blons::Graphics>(info);
    auto gui = graphics->gui();
    std::vector<std::unique_ptr<blons::Model>> models;

    // Model 1
    models.push_back(graphics->MakeModel("teapot_highpoly.bms"));
    models[0]->set_pos(0.0, 0.0, 20.0);

    // Model 2
    models.push_back(graphics->MakeModel("cube.bms"));
    models[1]->set_pos(10.0, 0.0, 20.0);

    // Model 3
    models.push_back(graphics->MakeModel("bumpy_cube/mesh/bumpy_cube.bms"));
    models[2]->set_pos(20.0, 0.0, 20.0);

    // Model 4
    models.push_back(graphics->MakeModel("plane.bms"));
    models[3]->set_pos(30.0, 0.0, 20.0);

    // Model 5
    models.push_back(graphics->MakeModel("blons:sphere"));
    models[4]->set_pos(0.0, 5.0, 0.0);

    // Big scene
    models = blons::temp::load_codmap("bms_test2uv", std::move(models), graphics.get());

    graphics->BuildLighting();

    // Sprite 1
    auto sprite = graphics->MakeSprite("me.dds");
    sprite->set_pos(0, 0, 32, 32);

    InitTestConsole(graphics.get(), info);

    bool quit = false;
    while (!quit)
    {
        quit = client->Frame();
        blons::temp::FPS();
        bool gui_used_input = gui->Update(*client->input());
        // Handles mouselook and wasd movement
        if (!gui_used_input)
        {
            blons::temp::noclip(client->input(), graphics->camera());
        }
        graphics->Render();
    }

    return 0;
}

void InitTestUI(blons::gui::Manager* gui)
{
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

    gui->window("yoyo")->MakeButton(10, 250, 280, 40, "New stuff maybe!");
}

void InitTestConsole(blons::Graphics* graphics, blons::Client::Info info)
{
    blons::console::out("Welcome, gamer -- to the blonstech universe!\n");

    std::function<void(int)> test_func = [](int i){ blons::console::out("%i gamers in the house!\n", i); };
    std::function<void(const char*)> test_func_s = [](const char* i){ blons::console::out("%s stringers in the house!\n", i); };
    std::function<void(int, int)> test_func_ii = [](int i, int j){ blons::console::out("%i, %i double inters in the house!\n", i, j); };
    blons::console::RegisterFunction("dbg:testo", test_func);
    blons::console::RegisterFunction("dbg:testo", test_func_s);
    blons::console::RegisterFunction("dbg:testo", test_func_ii);

    blons::console::RegisterVariable("sv:cool", 5);
    blons::console::RegisterVariable("math:pi", 3.14f);
    blons::console::RegisterVariable("sv:greeting", "heyo heyo heyo heyo");

    blons::console::set_var("sv:cool", 10);

    auto v_a = blons::console::var<int>("sv:cool");
    auto v_b = blons::console::var<float>("math:pi");
    auto v_c = blons::console::var<std::string>("sv:greeting");

    blons::console::RegisterFunction("gfx:reload", [=](){ graphics->Reload(info); });

    blons::console::RegisterFunction("dbg:test-ui", std::bind(InitTestUI, graphics->gui()));

    blons::console::RegisterFunction("dbg:console-history", [&]()
    {
        for (const auto& line : blons::console::history())
        {
            blons::console::out("%s\n", line.c_str());
        }
    });
}