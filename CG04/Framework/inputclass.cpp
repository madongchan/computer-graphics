// inputclass.cpp
#include "inputclass.h"
#include <windows.h>
#include <cstring>

// static 멤버 변수 초기화
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

// 기존 함수들 (호환성을 위해 유지하지만 사용하지 않음)
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

    // 이전 상태 저장
    memcpy(s_prevKeyState, s_currentKeyState, sizeof(s_currentKeyState));

    // 현재 상태 업데이트 (GetAsyncKeyState 사용)
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
