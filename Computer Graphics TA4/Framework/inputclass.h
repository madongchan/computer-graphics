// inputclass.h
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
public:
    InputClass();
    InputClass(const InputClass&);
    ~InputClass();

    void Initialize();

    // 기존 함수들 (호환성을 위해 유지)
    void KeyDown(unsigned int);
    void KeyUp(unsigned int);
    bool IsKeyDown(unsigned int);

    // 새로운 static 함수들
    static void UpdateKeyStates();
    static bool IsAnyKeyPressed();
    static bool IsKeyPressed(unsigned int keyCode);

private:
    bool m_keys[256];

    // static 멤버 변수들
    static bool s_currentKeyState[256];
    static bool s_prevKeyState[256];
    static bool s_initialized;
};

#endif
