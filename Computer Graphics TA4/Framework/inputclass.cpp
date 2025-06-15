#include "inputclass.h"
#include <windows.h>
#include <cstring>

bool InputClass::s_currentKeyState[256] = { false };
bool InputClass::s_prevKeyState[256] = { false };
bool InputClass::s_initialized = false;

int InputClass::s_mouseX = 0;
int InputClass::s_mouseY = 0;
int InputClass::s_prevMouseX = 0;
int InputClass::s_prevMouseY = 0;

InputClass::InputClass() 
{
    // 멤버 변수 초기화
    
}
InputClass::InputClass(const InputClass& other) {}
InputClass::~InputClass() {}

void InputClass::Initialize()
{
    for (int i = 0; i < 256; i++)
    {
        m_keys[i] = false;
    }
}

void InputClass::KeyDown(unsigned int input)
{
    m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
    m_keys[input] = false;
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

    // 현재 상태를 이전 상태로 복사 (중요!)
    memcpy(s_prevKeyState, s_currentKeyState, sizeof(s_currentKeyState));

    // 새로운 현재 상태 업데이트
    for (int i = 0; i < 256; ++i)
    {
        s_currentKeyState[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }

    // 마우스 위치 업데이트 (기존 코드)
    s_prevMouseX = s_mouseX;
    s_prevMouseY = s_mouseY;

    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(GetForegroundWindow(), &mousePos);
    s_mouseX = mousePos.x;
    s_mouseY = mousePos.y;
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
    return s_currentKeyState[keyCode];
}

void InputClass::UpdateMousePosition(int x, int y)
{
    s_prevMouseX = s_mouseX;
    s_prevMouseY = s_mouseY;
    s_mouseX = x;
    s_mouseY = y;
}
bool InputClass::IsKeyJustPressed(unsigned int keyCode)
{
    return s_currentKeyState[keyCode] && !s_prevKeyState[keyCode];
}

bool InputClass::IsKeyJustReleased(unsigned int keyCode)
{
    return !s_currentKeyState[keyCode] && s_prevKeyState[keyCode];
}

bool InputClass::IsAnyKeyJustPressed()
{
    for (int i = 0; i < 256; ++i)
    {
        // 특수 키들 제외 (마우스 버튼, 시스템 키 등)
        if (i == VK_LBUTTON || i == VK_RBUTTON || i == VK_MBUTTON ||
            i == VK_CAPITAL || i == VK_NUMLOCK || i == VK_SCROLL)
            continue;

        if (s_currentKeyState[i] && !s_prevKeyState[i])
        {
            return true;
        }
    }
    return false;
}

int InputClass::GetMouseDeltaX()
{
    return s_mouseX - s_prevMouseX;
}

int InputClass::GetMouseDeltaY()
{
    return s_mouseY - s_prevMouseY;
}
