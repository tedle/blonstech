#ifndef __INPUTTEMP_H__
#define __INPUTTEMP_H__

class CInput
{

public:
    CInput();
    ~CInput();

    bool Init();

    void KeyDown(unsigned int);
    void KeyUp(unsigned int);

    bool IsKeyDown(unsigned int);

    bool Frame();
    void Finish();

private:
    bool m_keys[256];
};

#endif