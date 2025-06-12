#pragma once

#include <windows.h>

class TimerClass
{
public:
    TimerClass();
    ~TimerClass();

    // ���α׷� ���� �� �� �� ȣ��
    void Initialize();

    // �� ������ ȣ�� �� ���� ������ ��� ��Ÿ Ÿ��(��)�� ��ȯ
    double GetDeltaTime();

private:
    __int64 m_frequency;
    __int64 m_lastTime;
};
