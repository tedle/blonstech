#ifndef BLONSTECH_INPUTTEMP_H_
#define BLONSTECH_INPUTTEMP_H_

#include <windowsx.h>
class Input
{

public:
    Input();
    ~Input();

    bool Init();

    void KeyDown(unsigned int);
    void KeyUp(unsigned int);

    bool IsKeyDown(unsigned int);

    void MouseDown(unsigned int);
    void MouseUp(unsigned int);

    bool IsMouseDown(unsigned int);

    void MouseMove(int, int);
    int MouseX();
    int MouseY();
    int MouseDeltaX();
    int MouseDeltaY();

    bool Frame();
    void Finish();

private:
    bool keys_[256];
    int mouse_x_, mouse_y_, old_mouse_x_, old_mouse_y_, delta_mouse_x_, delta_mouse_y_;
    bool buttons_[2];
};

#endif