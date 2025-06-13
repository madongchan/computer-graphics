#include "fpsclass.h"

FPSClass::FPSClass()
    : m_timer(nullptr)
    , m_frameCount(0)
    , m_fps(0)
    , m_timeElapsed(0.0)
{
}

FPSClass::~FPSClass()
{
}

void FPSClass::Initialize(TimerClass* timer)
{
    m_timer = timer;
    m_frameCount = 0;
    m_fps = 0;
    m_timeElapsed = 0.0;
}

void FPSClass::Frame()
{
    // ��Ÿ Ÿ���� ������ ����
    double delta = m_timer->GetDeltaTime();
    m_timeElapsed += delta;
    m_frameCount++;

    // 1�ʰ� ������ FPS ���
    if (m_timeElapsed >= 1.0)
    {
        m_fps = m_frameCount;
        m_frameCount = 0;
        m_timeElapsed = 0.0;
    }
}

int FPSClass::GetFPS() const
{
    return m_fps;
}
