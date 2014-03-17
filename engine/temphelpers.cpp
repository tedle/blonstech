#include "temphelpers.h"


void noclip(Input* input, Camera* camera)
{
    // Always handle mouse look before wasd movement!
    if (input->IsMouseDown(0))
    {
        Vector3 rot = camera->GetRot();
        rot.x += input->MouseDeltaY() * 0.003;
        rot.y += input->MouseDeltaX() * 0.003;
        camera->SetRot(rot.x, rot.y, 0.0);
    }

    static DWORD last_move = 0;
    // Welcome to the hackiest no clip wasd movement u ever see
    if (GetTickCount() > last_move + 10)
    {
        float velocity = 0.2;
        if (input->IsKeyDown(VK_SHIFT))
        {
            velocity *= 0.25;
        }
        if (input->IsKeyDown(VK_CONTROL))
        {
            velocity *= 4.0;
        }
        last_move = GetTickCount();
        Vector3 pos = camera->GetPos();
        Vector3 rot = camera->GetRot();
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
        float yaw = atan2(0, ud) + rot.y;
        float pitch = rot.x;

        // since im awful with math, this is how i make us not move faster when u/d + l/r are held together
        float hacky = (lr == 0 ? 1.0 : 1.0 / sqrt(2));
        // ud!=0 to make sure we dont move forward/back if only A/D are held
        float new_x = velocity * sin(yaw) * cos(pitch) * hacky * (float)(ud!=0);
        float new_y = velocity * -1.0 * (float)ud * hacky * sin(pitch);
        float new_z = velocity * cos(yaw) * cos(pitch) * hacky * (float)(ud!=0);

        yaw = atan2(lr, 0) + rot.y;

        // sorry, still bad at match
        hacky = (ud == 0 ? 1.0 : 1.0 / sqrt(2));
        // lr!=0 to make sure we dont move left/right if only W/S are held
        new_x += velocity * sin(yaw) * hacky * (float)(lr!=0);
        new_z += velocity * cos(yaw) * hacky * (float)(lr!=0);

        // in case we wanna go up/down w/o flailing mouse around
        if (input->IsKeyDown('Q'))
        {
            new_y += velocity;
        }
        if (input->IsKeyDown('E'))
        {
            new_y -= velocity;
        }

        camera->SetPos(pos.x+new_x, pos.y+new_y, pos.z+new_z);
    }
}

void move_camera_around_origin(float delta, Camera* camera)
{
    float orientation = static_cast<float>(XM_PI)*1.5f;
    static DWORD last_call = 0;

    if (GetTickCount() > last_call+10)
    {
        last_call = GetTickCount();

        float r = 20.0f;
        float orient_delta = 0.03f * delta;
        orientation += orient_delta;

        float x = r * cos(orientation);
        float y = 10.0f;
        float z = r * sin(orientation);

        camera->SetPos(x, y, z);
        camera->LookAt(0.0f, 2.0f, 0.0f);
        //camera->SetPos(0.0f, -2000.0f, 500.0f);
        //camera->LookAt(0.0f, 0.0f, 0.0f);
    }
}

DWORD last_time = 0;
int fps_count = 0;
void FPS()
{
    DWORD st = GetTickCount();

    if (st > last_time+1000)
    {
        char msg[64];
        sprintf_s(msg, "FPS: %i\n", fps_count);
        OutputDebugStringA(msg);
        last_time = st;
        fps_count = 0;
    }
    else
        fps_count++;
}
