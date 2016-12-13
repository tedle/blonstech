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
void SetRenderingOutput(blons::Graphics* graphics);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
    auto client = std::make_unique<blons::Client>();
    auto info = client->screen_info();
    blons::render::MakeContext(info);
    auto graphics = std::make_unique<blons::Graphics>(info);
    auto gui = graphics->gui();
    std::vector<std::unique_ptr<blons::Model>> models;

    // Big scene
    models = blons::temp::load_codmap("bms_test2uv", std::move(models), graphics.get());

    // Model 1
    models.push_back(graphics->MakeModel("teapot_highpoly.bms"));
    models.back()->set_pos(0.0, 0.0, 20.0);

    // Model 2
    models.push_back(graphics->MakeModel("cube.bms"));
    models.back()->set_pos(10.0, 0.0, 20.0);

    // Model 3
    models.push_back(graphics->MakeModel("bumpy_cube/mesh/bumpy_cube.bms"));
    models.back()->set_pos(20.0, 0.0, 20.0);

    // Model 4
    models.push_back(graphics->MakeModel("plane.bms"));
    models.back()->set_pos(30.0, 0.0, 20.0);

    // Model 5
    models.push_back(graphics->MakeModel("blons:sphere~0.5"));
    models.back()->set_pos(0.0, 5.0, 0.0);

    graphics->BakeRadianceTransfer();

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
        SetRenderingOutput(graphics.get());
        graphics->Render();
    }

    return 0;
}

void InitTestUI(blons::gui::Manager* gui)
{
    gui->MakeWindow("yoyo", 450, 250, 300, 370, "Amicable window");
    gui->MakeWindow("test", 20, 80, 400, 270, "Friendly window");

    auto textarea = gui->window("yoyo")->MakeTextarea(10, 110, 280, 205);
    for (int i = 0; i < 5; i++)
    {
        textarea->AddLine("Simple test line that isn't too short,\nmaybe even a little long, thanks for the friendly test!");
    }

    textarea->AddLine("HAello! blonsUI in action!");

    auto textbox = gui->window("test")->MakeTextbox(135, 220, 255, 40);
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
    gui->window("test")->MakeButton(10, 220, 120, 40, "Print!")->set_callback(print);
    gui->window("test")->MakeButton(10, 175, 380, 40, "Clear!")->set_callback(textareaclear);

    gui->window("yoyo")->MakeButton(10, 320, 280, 40, "New stuff maybe!");

    gui->window("test")->MakeDebugSlider(10, 110, 380, 40, -5, 20, 0.01f);
}

void InitTestConsole(blons::Graphics* graphics, blons::Client::Info info)
{
    blons::console::out("Welcome, gamer -- to the blonstech universe!\n");

    blons::console::RegisterFunction("dbg:testo", [](int i){ blons::console::out("%i gamers in the house!\n", i); });
    blons::console::RegisterFunction("dbg:testo", [](const char* i){ blons::console::out("%s stringers in the house!\n", i); });
    blons::console::RegisterFunction("dbg:testo", [](int i, int j){ blons::console::out("%i, %i double inters in the house!\n", i, j); });

    blons::console::RegisterVariable("sv:cool", 5);
    blons::console::RegisterVariable("math:pi", 3.14f);
    blons::console::RegisterVariable("sv:greeting", "heyo heyo heyo heyo");

    blons::console::set_var("sv:cool", 10);

    auto v_a = blons::console::var<int>("sv:cool");
    auto v_b = blons::console::var<float>("math:pi");
    auto v_c = blons::console::var<std::string>("sv:greeting");

    blons::console::RegisterFunction("gfx:reload", [=](){ graphics->Reload(info); });
    blons::console::RegisterVariable("gfx:target", 0);
    blons::console::RegisterVariable("gfx:alt-target", 1);

    blons::console::RegisterFunction("dbg:test-ui", std::bind(InitTestUI, graphics->gui()));

    blons::console::RegisterFunction("dbg:console-history", [&]()
    {
        for (const auto& line : blons::console::history())
        {
            blons::console::out("%s\n", line.c_str());
        }
    });
}

void SetRenderingOutput(blons::Graphics* graphics)
{
    static const blons::console::Variable* target = blons::console::var("gfx:target");
    static const blons::console::Variable* alt_target = blons::console::var("gfx:alt-target");
    auto get_target = [](int target)
    {
        switch (target)
        {
        case 1:
            return blons::pipeline::Deferred::ALBEDO;
            break;
        case 2:
            return blons::pipeline::Deferred::NORMAL;
            break;
        case 3:
            return blons::pipeline::Deferred::DEBUG;
            break;
        case 4:
            return blons::pipeline::Deferred::G_DEPTH;
            break;
        case 5:
            return blons::pipeline::Deferred::LIGHT_DEPTH;
            break;
        case 6:
            return blons::pipeline::Deferred::DIRECT_LIGHT;
            break;
        case 7:
            return blons::pipeline::Deferred::PROBE_ENV_MAPS;
            break;
        case 0:
        default:
            return blons::pipeline::Deferred::FINAL;
            break;
        }
    };
    graphics->set_output(get_target(target->to<int>()), get_target(alt_target->to<int>()));
}