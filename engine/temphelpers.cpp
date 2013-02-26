#include "temphelpers.h"

float orientation = static_cast<float>(XM_PI)*1.5f;
DWORD last_call = 0;
void move_camera_around_origin(float delta, Camera* camera)
{
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
