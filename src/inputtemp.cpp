#include "input/inputtemp.h"

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
    if (!IsKeyDown(vk))
    {
        event_queue_buffer_.push_back(Event(Event::KEY_DOWN, vk));
    }
    keys_[vk] = true;
    return;
}

void Input::KeyUp(unsigned int vk)
{
    if (IsKeyDown(vk))
    {
        event_queue_buffer_.push_back(Event(Event::KEY_UP, vk));
    }
    keys_[vk] = false;
    return;
}

bool Input::IsKeyDown(unsigned int vk) const
{
    return keys_[vk];
}

void Input::MouseDown(unsigned int vk)
{
    if (!IsMouseDown(vk))
    {
        event_queue_buffer_.push_back(Event(Event::MOUSE_DOWN, vk));
    }
    buttons_[vk] = true;
    return;
}

void Input::MouseUp(unsigned int vk)
{
    if (IsMouseDown(vk))
    {
        event_queue_buffer_.push_back(Event(Event::MOUSE_UP, vk));
    }
    buttons_[vk] = false;
    return;
}

bool Input::IsMouseDown(unsigned int vk) const
{
    return buttons_[vk];
}

void Input::MouseMove(int x, int y)
{
    mouse_x_ = x;
    mouse_y_ = y;
}

int Input::MouseX() const
{
    return mouse_x_;
}

int Input::MouseY() const
{
    return mouse_y_;
}

int Input::MouseDeltaX() const
{
    return delta_mouse_x_;
}

int Input::MouseDeltaY() const
{
    return delta_mouse_y_;
}

const std::vector<Input::Event>& Input::EventQueue() const
{
    return event_queue_;
}

bool Input::Frame()
{
    delta_mouse_x_ = mouse_x_ - old_mouse_x_;
    delta_mouse_y_ = mouse_y_ - old_mouse_y_;
    old_mouse_x_ = mouse_x_;
    old_mouse_y_ = mouse_y_;

    if (delta_mouse_x_ != 0)
    {
        event_queue_buffer_.push_back(Event(Event::MOUSE_MOVE_X, delta_mouse_x_));
    }
    if (delta_mouse_y_ != 0)
    {
        event_queue_buffer_.push_back(Event(Event::MOUSE_MOVE_Y, delta_mouse_y_));
    }

    // Since event polling is done before frame is called
    event_queue_ = event_queue_buffer_;
    event_queue_buffer_.clear();

    return true;
}
} // namespace blons
