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

#include <blons/temphelpers.h>

// Includes
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
// Public Includes
#include <blons/graphics/camera.h>
#include <blons/graphics/graphics.h>
#include <blons/graphics/model.h>
#include <blons/input/inputtemp.h>
#include <blons/system/timer.h>

namespace blons
{
namespace temp
{
Vector3 cur_pos(0.0, 0.0, 0.0);

void noclip(Input* input, Camera* camera)
{
    // Always handle mouse look before wasd movement!
    if (input->IsMouseDown(0))
    {
        Vector3 rot = camera->rot();
        rot.x -= input->mouse_delta_y() * 0.003f;
        rot.y -= input->mouse_delta_x() * 0.003f;
        camera->set_rot(rot.x, rot.y, 0.0);
    }

    static Timer last_move;
    // Welcome to the hackiest no clip wasd movement u ever see
    if (last_move.ms() > 10)
    {
        last_move.rewind(10);

        float velocity = 0.2f;
        if (input->IsKeyDown(Input::SHIFT))
        {
            velocity *= 0.25f;
        }
        if (input->IsKeyDown(Input::CONTROL))
        {
            velocity *= 4.0f;
        }
        Vector3 pos = camera->pos();
        Vector3 rot = camera->rot();
        int ud = 0, lr = 0;

        // Figure out which way we wanna move, to be plugged into atan2
        if (input->IsKeyDown(Input::CHAR_W))
        {
            ud -= 1;
        }
        if (input->IsKeyDown(Input::CHAR_S))
        {
            ud += 1;
        }
        if (input->IsKeyDown(Input::CHAR_A))
        {
            lr -= 1;
        }
        if (input->IsKeyDown(Input::CHAR_D))
        {
            lr += 1;
        }

        // We do ud and lr separately because lr is NOT affected by pitch, but ud is
        float yaw = static_cast<float>(atan2(0, ud) + rot.y);
        float pitch = rot.x;

        // since im awful with math, this is how i make us not move faster when u/d + l/r are held together
        float hacky = static_cast<float>(lr == 0 ? 1.0 : 1.0 / sqrt(2));
        // ud!=0 to make sure we dont move forward/back if only A/D are held
        float new_x = velocity * sin(yaw) * cos(pitch) * hacky * (float)(ud != 0);
        float new_y = velocity * -1.0f * (float)ud * hacky * sin(pitch);
        float new_z = velocity * cos(yaw) * cos(pitch) * hacky * (float)(ud != 0);

        yaw = static_cast<float>(atan2(lr, 0) + rot.y);

        // sorry, still bad at match
        hacky = static_cast<float>(ud == 0 ? 1.0 : 1.0 / sqrt(2));
        // lr!=0 to make sure we dont move left/right if only W/S are held
        new_x += velocity * sin(yaw) * hacky * (float)(lr != 0);
        new_z += velocity * cos(yaw) * hacky * (float)(lr != 0);

        // in case we wanna go up/down w/o flailing mouse around
        if (input->IsKeyDown(Input::CHAR_E))
        {
            new_y += velocity;
        }
        if (input->IsKeyDown(Input::CHAR_Q))
        {
            new_y -= velocity;
        }

        camera->set_pos(pos.x + new_x, pos.y + new_y, pos.z + new_z);
        cur_pos = camera->pos();
    }
}

void move_camera_around_origin(float delta, Camera* camera)
{
    float orientation = static_cast<float>(XM_PI)*1.5f;
    static Timer last_call;

    if (last_call.ms() > 10)
    {
        last_call.rewind(10);

        float r = 20.0f;
        float orient_delta = 0.03f * delta;
        orientation += orient_delta;

        float x = r * cos(orientation);
        float y = 10.0f;
        float z = r * sin(orientation);

        camera->set_pos(x, y, z);
        camera->LookAt(0.0f, 2.0f, 0.0f);
    }
}

void FPS()
{
    static DWORD64 last_frame = 0;
    static DWORD64 max_frame = 0;
    static int fps_count = 0;
    static Timer st;

    max_frame = max(max_frame, st.ms() - last_frame);

    if (st.ms() > 1000 && max_frame > 0)
    {
        log::Debug("FPS: %i(min:%llu), (x=%.2f,y=%.2f,z=%.2f)\n", fps_count, 1000 / max_frame, cur_pos.x, cur_pos.y, cur_pos.z);
        max_frame = 0;
        fps_count = 0;
        st.rewind(1000);
    }
    else
    {
        fps_count++;
    }
    last_frame = st.ms();
}


std::vector<std::unique_ptr<Model>> load_codmap(std::string folder, std::vector<std::unique_ptr<Model>> models, Graphics* graphics)
{
    Timer timer;
    std::string csv_file = folder;
    if (csv_file.back() != '/' && csv_file.back() != '\\')
    {
        csv_file += "/";
    }
    std::string mesh_folder = csv_file + "mesh/";
    std::string tex_folder = csv_file + "tex/";
    csv_file += "list.csv";
    std::ifstream csv(csv_file.c_str(), std::ios::in);
    if (!csv.is_open())
    {
        throw "csv open problem";
    }
    std::string line;
    while (std::getline(csv, line))
    {
        std::string mesh_file = mesh_folder + line.substr(0, line.find(','));
        std::string tex_file = tex_folder + line.substr(line.find(',') + 1);

        if (mesh_file.size() == 0 || tex_file.size() == 0)
        {
            throw "csv read problem";
        }

        models.push_back(graphics->MakeModel(mesh_file.c_str()));
        if (models.back() == nullptr)
        {
            throw "model problem";
        }
        models.back()->set_pos(0.0, 0.0, 0.0);
    }
    /*models.push_back(std::unique_ptr<Model>(new Model(L"codmap.mesh", L"me.dds")));
    if (models[1] == nullptr)
    {
        throw "model problem";
    }

    if (!models[1]->Init(L"codmap.mesh", L"me.dds"))
    {
        throw "other model problem";
    }
    models[1]->set_pos(0.0, 0.0, 0.0);*/
    log::Debug("Loaded map [%ims]\n", timer.ms());
    return models;
}
} // namespace temp
} // namespace blons
