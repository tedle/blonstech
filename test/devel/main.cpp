////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
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
    models = blons::temp::load_batch_models("old_sponza_2uv", std::move(models), graphics.get());

    // Model 1
    models.push_back(graphics->MakeModel("teapot_highpoly.bms"));
    models.back()->set_pos(0.0, 0.0, 20.0);

    // Model 2
    models.push_back(graphics->MakeModel("cube.bms"));
    models.back()->set_pos(10.0, 0.0, 20.0);

    // Model 3
    models.push_back(graphics->MakeModel("plane.bms"));
    models.back()->set_pos(30.0, 0.0, 20.0);

    // Model 4
    models.push_back(graphics->MakeModel("blons:sphere"));
    models.back()->set_pos(0.0, 5.0, 0.0);
    models.back()->set_scale(0.5, 0.5, 0.5);

    graphics->BakeRadianceTransfer();

    InitTestConsole(graphics.get(), info);

    bool quit = false;
    while (!quit)
    {
        quit = client->Frame();
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
    auto win1 = gui->MakeWindow(450, 250, 300, 370, "Amicable window", blons::gui::Window::Type::DRAGGABLE);
    auto win2 = gui->MakeWindow(20, 80, 400, 270, "Friendly window", blons::gui::Window::Type::DRAGGABLE);

    auto textarea = win1->MakeTextarea(10, 110, 280, 205);
    for (int i = 0; i < 5; i++)
    {
        textarea->AddLine("Simple test line that isn't too short,\nmaybe even a little long, thanks for the friendly test!");
    }

    textarea->AddLine("Hello! blonstech UI in action!");

    auto textbox = win2->MakeTextbox(135, 220, 255, 40);
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
    win2->MakeButton(10, 220, 120, 40, "Print!")->set_callback(print);
    win2->MakeButton(10, 175, 380, 40, "Clear!")->set_callback(textareaclear);

    win1->MakeButton(10, 320, 280, 40, "Button Without Purpose");

    win2->MakeDebugSlider(10, 110, 380, 40, -5, 20, 0.01f);
}

void InitTestConsole(blons::Graphics* graphics, blons::Client::Info info)
{
    blons::console::out("blonstech console initialized\n");

    blons::console::RegisterFunction("main:test", [](int i){ blons::console::out("%i (int callback)\n", i); });
    blons::console::RegisterFunction("main:test", [](const char* i){ blons::console::out("%s (string callback)\n", i); });
    blons::console::RegisterFunction("main:test", [](int i, int j){ blons::console::out("%i, %i (int, int callback)\n", i, j); });

    blons::console::RegisterVariable("sv:test", 5);
    blons::console::RegisterVariable("math:pi", 3.14f);
    blons::console::RegisterVariable("sv:greeting", "Greetings!");

    blons::console::set_var("sv:test", 10);

    auto v_a = blons::console::var<int>("sv:test");
    auto v_b = blons::console::var<float>("math:pi");
    auto v_c = blons::console::var<std::string>("sv:greeting");

    blons::console::RegisterFunction("gfx:reload", [=](){ graphics->Reload(info); graphics->BakeRadianceTransfer(); });
    blons::console::RegisterVariable("dbg:target", 0);
    blons::console::RegisterVariable("dbg:alt-target", 1);

    blons::console::RegisterFunction("main:test-ui", std::bind(InitTestUI, graphics->gui()));

    blons::console::RegisterFunction("con:history", [&]()
    {
        for (const auto& line : blons::console::history())
        {
            blons::console::out("%s\n", line.c_str());
        }
    });
}

void SetRenderingOutput(blons::Graphics* graphics)
{
    static const blons::console::Variable* target = blons::console::var("dbg:target");
    static const blons::console::Variable* alt_target = blons::console::var("dbg:alt-target");
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
            return blons::pipeline::Deferred::G_DEPTH;
            break;
        case 4:
            return blons::pipeline::Deferred::LIGHT_DEPTH;
            break;
        case 5:
            return blons::pipeline::Deferred::DIRECT_LIGHT;
            break;
        case 6:
            return blons::pipeline::Deferred::LIGHT;
            break;
        case 0:
        default:
            return blons::pipeline::Deferred::FINAL;
            break;
        }
    };
    graphics->set_output(get_target(target->to<int>()), get_target(alt_target->to<int>()));
}