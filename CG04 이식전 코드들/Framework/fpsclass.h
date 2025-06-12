#pragma once

#include "timerclass.h"

class FPSClass
{
public:
    FPSClass();
    ~FPSClass();

    // Ÿ�̸� �����͸� �޾� �ʱ�ȭ
    void Initialize(TimerClass* timer);

    // �� �������� ���� ������ ȣ��, ���ο��� 1�ʸ��� FPS ���
    void Frame();

    // ���� ���� FPS �� ��ȯ
    int GetFPS() const;

private:
    TimerClass* m_timer;
    int m_frameCount;      // ������ ������ ��
    int m_fps;             // ���������� ���� FPS
    double m_timeElapsed;  // ���������� FPS�� ����� ���� ������ �ð�
};
