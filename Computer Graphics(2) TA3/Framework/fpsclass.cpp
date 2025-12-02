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

void FPSClass::Frame(double deltaTime)
{
    m_frameCount++;

    // m_timeElapsed += m_timer->GetDeltaTime(); // <-- 이 줄을 삭제하거나 주석 처리!
    m_timeElapsed += deltaTime;                  // <-- 이렇게 변경!

    if (m_timeElapsed >= 1.0)
    {
        m_fps = m_frameCount;
        m_frameCount = 0;
        m_timeElapsed -= 1.0;
    }
}

int FPSClass::GetFPS() const
{
    return m_fps;
}
