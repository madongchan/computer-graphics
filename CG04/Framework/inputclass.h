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

    // ���� �Լ��� (ȣȯ���� ���� ����)
    void KeyDown(unsigned int);
    void KeyUp(unsigned int);
    bool IsKeyDown(unsigned int);

    // ���ο� static �Լ���
    static void UpdateKeyStates();
    static bool IsAnyKeyPressed();
    static bool IsKeyPressed(unsigned int keyCode);

private:
    bool m_keys[256];

    // static ��� ������
    static bool s_currentKeyState[256];
    static bool s_prevKeyState[256];
    static bool s_initialized;
};

#endif
