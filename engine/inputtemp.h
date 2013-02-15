#ifndef __INPUTTEMP_H__
#define __INPUTTEMP_H__

class Input
{

public:
    Input();
    ~Input();

    bool Init();

    void KeyDown(unsigned int);
    void KeyUp(unsigned int);

    bool IsKeyDown(unsigned int);

    bool Frame();
    void Finish();

private:
    bool keys_[256];
};

#endif