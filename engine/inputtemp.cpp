#include "inputtemp.h"

Input::Input()
{
}

Input::~Input()
{
}

bool Input::Init()
{
    for (int i=0; i<256; i++)
        keys_[i] = false;

    return true;
}

void Input::KeyDown(unsigned int vk)
{
    keys_[vk] = true;
    return;
}

void Input::KeyUp(unsigned int vk)
{
    keys_[vk] = false;
    return;
}

bool Input::IsKeyDown(unsigned int vk)
{
    return keys_[vk];
}

bool Input::Frame()
{
    return true;
}

void Input::Finish()
{
    return;
}