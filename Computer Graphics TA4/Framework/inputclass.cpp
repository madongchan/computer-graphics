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

InputClass::InputClass() {}
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

    for (int i = 0; i < 256; ++i)
    {
        s_currentKeyState[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }

    // Update previous mouse position
    s_prevMouseX = s_mouseX;
    s_prevMouseY = s_mouseY;

    // Get current mouse position
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

int InputClass::GetMouseDeltaX()
{
    return s_mouseX - s_prevMouseX;
}

int InputClass::GetMouseDeltaY()
{
    return s_mouseY - s_prevMouseY;
}
