#include "inputtemp.h"

CInput::CInput()
{
}

CInput::~CInput()
{
}

bool CInput::Init()
{
    for(int i=0; i<256; i++)
        m_keys[i] = false;

    return true;
}

void CInput::KeyDown(unsigned int vk)
{
    m_keys[vk] = true;
    return;
}

void CInput::KeyUp(unsigned int vk)
{
    m_keys[vk] = false;
    return;
}

bool CInput::IsKeyDown(unsigned int vk)
{
    return m_keys[vk];
}

bool CInput::Frame()
{
    return true;
}

void CInput::Finish()
{
    return;
}