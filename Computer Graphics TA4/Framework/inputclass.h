#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
public:
    InputClass();
    InputClass(const InputClass&);
    ~InputClass();

    void Initialize();
    void KeyDown(unsigned int);
    void KeyUp(unsigned int);
    bool IsKeyDown(unsigned int);

    static void UpdateKeyStates();
    static bool IsAnyKeyPressed();
    static bool IsKeyPressed(unsigned int keyCode);

   

    static void UpdateMousePosition(int x, int y);
    bool IsKeyJustPressed(unsigned int keyCode);
    bool IsKeyJustReleased(unsigned int keyCode);
    static bool IsAnyKeyJustPressed();
    static int GetMouseDeltaX();
    static int GetMouseDeltaY();

private:
    bool m_keys[256];

    static bool s_currentKeyState[256];
    static bool s_prevKeyState[256];
    static bool s_initialized;

    static int s_mouseX, s_mouseY;
    static int s_prevMouseX, s_prevMouseY;
};

#endif
