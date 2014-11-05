#include "inputtemp.h"

namespace blons
{
Input::Input()
{
    for (int i=0; i<256; i++)
        keys_[i] = false;
    for (int i=0; i<2; i++)
        buttons_[i] = false;

    mouse_x_ = mouse_y_ = old_mouse_x_ = old_mouse_y_ = delta_mouse_x_ = delta_mouse_y_ = 0;
}

Input::~Input()
{
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

void Input::MouseDown(unsigned int vk)
{
    buttons_[vk] = true;
    return;
}

void Input::MouseUp(unsigned int vk)
{
    buttons_[vk] = false;
    return;
}

bool Input::IsMouseDown(unsigned int vk)
{
    return buttons_[vk];
}

void Input::MouseMove(int x, int y)
{
    mouse_x_ = x;
    mouse_y_ = y;
}

int Input::MouseX()
{
    return mouse_x_;
}

int Input::MouseY()
{
    return mouse_y_;
}

int Input::MouseDeltaX()
{
    return delta_mouse_x_;
}

int Input::MouseDeltaY()
{
    return delta_mouse_y_;
}

bool Input::Frame()
{
    delta_mouse_x_ = mouse_x_ - old_mouse_x_;
    delta_mouse_y_ = mouse_y_ - old_mouse_y_;
    old_mouse_x_ = mouse_x_;
    old_mouse_y_ = mouse_y_;
    return true;
}
} // namespace blons
