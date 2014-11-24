#ifndef BLONSTECH_INPUT_INPUTTEMP_H_
#define BLONSTECH_INPUT_INPUTTEMP_H_

// Includes
#include <vector>
#include <windowsx.h>

namespace blons
{
class Input
{
public:
    struct Event
    {
        enum Type
        {
            MOUSE_DOWN,
            MOUSE_UP,
            MOUSE_MOVE_X,
            MOUSE_MOVE_Y,
            KEY_DOWN,
            KEY_UP
        } type;

        int value;

        Event(Type t, int v) : type(t), value(v) {}
    };

public:
    Input();
    ~Input();

    void KeyDown(unsigned int);
    void KeyUp(unsigned int);

    bool IsKeyDown(unsigned int) const;

    void MouseDown(unsigned int);
    void MouseUp(unsigned int);

    bool IsMouseDown(unsigned int) const;

    void MouseMove(int, int);
    int MouseX() const;
    int MouseY() const;
    int MouseDeltaX() const;
    int MouseDeltaY() const;
    const std::vector<Event>& Input::EventQueue() const;

    bool Frame();

private:
    bool keys_[256];
    int mouse_x_, mouse_y_, old_mouse_x_, old_mouse_y_, delta_mouse_x_, delta_mouse_y_;
    bool buttons_[5];

    std::vector<Event> event_queue_;
    std::vector<Event> event_queue_buffer_;
};
} // namespace blons

#endif