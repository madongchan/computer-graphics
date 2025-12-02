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
    // 델타 타임을 가져와 누적
    double delta = m_timer->GetDeltaTime();
    m_timeElapsed += delta;
    m_frameCount++;

    // 1초가 넘으면 FPS 계산
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
