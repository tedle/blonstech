#ifndef BLONSTECH_INPUTTEMP_H_
#define BLONSTECH_INPUTTEMP_H_

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