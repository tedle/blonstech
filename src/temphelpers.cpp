#include "temphelpers.h"

// Includes
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
// Local Includes
#include "graphics/camera.h"
#include "graphics/model.h"
#include "input/inputtemp.h"

namespace blons
{
Vector3 cur_pos(0.0, 0.0, 0.0);

void noclip(Input* input, Camera* camera)
{
    // Always handle mouse look before wasd movement!
    if (input->IsMouseDown(0))
    {
        Vector3 rot = camera->rot();
        rot.x += input->MouseDeltaY() * 0.003f;
        rot.y += input->MouseDeltaX() * 0.003f;
        camera->set_rot(rot.x, rot.y, 0.0);
    }

    static DWORD64 last_move = 0;
    // Welcome to the hackiest no clip wasd movement u ever see
    if (GetTickCount64() > last_move + 10)
    {
        float velocity = 0.2f;
        if (input->IsKeyDown(VK_SHIFT))
        {
            velocity *= 0.25f;
        }
        if (input->IsKeyDown(VK_CONTROL))
        {
            velocity *= 4.0f;
        }
        last_move = GetTickCount64();
        Vector3 pos = camera->pos();
        Vector3 rot = camera->rot();
        int ud = 0, lr = 0;

        // Figure out which way we wanna move, to be plugged into atan2
        if (input->IsKeyDown('W'))
        {
            ud += 1;
        }
        if (input->IsKeyDown('S'))
        {
            ud -= 1;
        }
        if (input->IsKeyDown('A'))
        {
            lr -= 1;
        }
        if (input->IsKeyDown('D'))
        {
            lr += 1;
        }

        // We do ud and lr separately because lr is NOT affected by pitch, but ud is
        float yaw = static_cast<float>(atan2(0, ud) + rot.y);
        float pitch = rot.x;

        // since im awful with math, this is how i make us not move faster when u/d + l/r are held together
        float hacky = static_cast<float>(lr == 0 ? 1.0 : 1.0 / sqrt(2));
        // ud!=0 to make sure we dont move forward/back if only A/D are held
        float new_x = velocity * sin(yaw) * cos(pitch) * hacky * (float)(ud!=0);
        float new_y = velocity * -1.0f * (float)ud * hacky * sin(pitch);
        float new_z = velocity * cos(yaw) * cos(pitch) * hacky * (float)(ud!=0);

        yaw = static_cast<float>(atan2(lr, 0) + rot.y);

        // sorry, still bad at match
        hacky = static_cast<float>(ud == 0 ? 1.0 : 1.0 / sqrt(2));
        // lr!=0 to make sure we dont move left/right if only W/S are held
        new_x += velocity * sin(yaw) * hacky * (float)(lr!=0);
        new_z += velocity * cos(yaw) * hacky * (float)(lr!=0);

        // in case we wanna go up/down w/o flailing mouse around
        if (input->IsKeyDown('E'))
        {
            new_y += velocity;
        }
        if (input->IsKeyDown('Q'))
        {
            new_y -= velocity;
        }

        camera->set_pos(pos.x+new_x, pos.y+new_y, pos.z+new_z);
        cur_pos = camera->pos();
    }
}

void move_camera_around_origin(float delta, Camera* camera)
{
    float orientation = static_cast<float>(XM_PI)*1.5f;
    static DWORD64 last_call = 0;

    if (GetTickCount64() > last_call+10)
    {
        last_call = GetTickCount64();

        float r = 20.0f;
        float orient_delta = 0.03f * delta;
        orientation += orient_delta;

        float x = r * cos(orientation);
        float y = 10.0f;
        float z = r * sin(orientation);

        camera->set_pos(x, y, z);
        camera->LookAt(0.0f, 2.0f, 0.0f);
        //camera->set_pos(0.0f, -2000.0f, 500.0f);
        //camera->LookAt(0.0f, 0.0f, 0.0f);
    }
}

void FPS()
{
    static DWORD64 last_second = 0;
    static DWORD64 last_frame = 0;
    static DWORD64 max_frame = 0;
    static int fps_count = 0;
    DWORD64 st = GetTickCount64();

    max_frame = max(max_frame, st - last_frame);

    if (st > last_second+1000)
    {
        g_log->Debug("FPS: %i(min:%llu), (x=%.2f,y=%.2f,z=%.2f)\n", fps_count, 1000/max_frame, cur_pos.x, cur_pos.y, cur_pos.z);
        last_second = st;
        max_frame = 0;
        fps_count = 0;
    }
    else
    {
        fps_count++;
    }
    last_frame = st;
}


std::vector<std::unique_ptr<Model>> load_codmap(const char* folder, std::vector<std::unique_ptr<Model>> models, RenderContext& context)
{
    DWORD64 start = GetTickCount64();
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
        std::string tex_file = tex_folder + line.substr(line.find(',')+1);
        
        if (mesh_file.size() == 0 || tex_file.size() == 0)
        {
            throw "csv read problem";
        }

        models.push_back(std::unique_ptr<Model>(new Model(mesh_file.c_str(), context)));
        if (models.back() == nullptr)
        {
            throw "model problem";
        }
        models.back()->set_pos(0.0, 0.0, 0.0);
    }
    /*models.push_back(std::unique_ptr<Model>(new Model(L"../notes/codmap.mesh", L"../notes/me.dds")));
    if (models[1] == nullptr)
    {
        throw "model problem";
    }

    if (!models[1]->Init(L"../../notes/codmap.mesh", L"../notes/me.dds"))
    {
        throw "other model problem";
    }
    models[1]->set_pos(0.0, 0.0, 0.0);*/
    DWORD64 end = GetTickCount64();
    g_log->Debug("Loaded map [%ims]\n", end - start);
    return models;
}
} // namespace blons
