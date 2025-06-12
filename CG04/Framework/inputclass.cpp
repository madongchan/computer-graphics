// inputclass.cpp
#include "inputclass.h"
#include <windows.h>
#include <cstring>

// static ��� ���� �ʱ�ȭ
bool InputClass::s_currentKeyState[256] = { false };
bool InputClass::s_prevKeyState[256] = { false };
bool InputClass::s_initialized = false;

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
    int i;
    for (i = 0; i < 256; i++)
    {
        m_keys[i] = false;
    }
    return;
}

// ���� �Լ��� (ȣȯ���� ���� ���������� ������� ����)
void InputClass::KeyDown(unsigned int input)
{
    m_keys[input] = true;
    return;
}

void InputClass::KeyUp(unsigned int input)
{
    m_keys[input] = false;
    return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
    return m_keys[key];
}

void InputClass::UpdateKeyStates()
{
    if (!s_initialized)
    {
        memset(s_currentKeyState, false, sizeof(s_currentKeyState));
        memset(s_prevKeyState, false, sizeof(s_prevKeyState));
        s_initialized = true;
    }

    // ���� ���� ����
    memcpy(s_prevKeyState, s_currentKeyState, sizeof(s_currentKeyState));

    // ���� ���� ������Ʈ (GetAsyncKeyState ���)
    for (int i = 0; i < 256; ++i)
    {
        s_currentKeyState[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
}

bool InputClass::IsAnyKeyPressed()
{
    for (int i = 0; i < 256; ++i)
    {
        if (s_currentKeyState[i] && !s_prevKeyState[i])
        {
            return true;
        }
    }
    return false;
}

bool InputClass::IsKeyPressed(unsigned int keyCode)
{
    return s_currentKeyState[keyCode] && !s_prevKeyState[keyCode];
}
