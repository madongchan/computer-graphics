#include "timerclass.h"

TimerClass::TimerClass()
    : m_frequency(0)
    , m_lastTime(0)
{
}

TimerClass::~TimerClass()
{
}

void TimerClass::Initialize()
{
    // QueryPerformanceFrequency 로 카운터 주파수를 구해서 m_frequency 에 저장
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));

    // QueryPerformanceCounter 로 현재 카운터 값을 m_lastTime 에 저장
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_lastTime));
}

double TimerClass::GetDeltaTime()
{
    // 방어: 초기화 안 된 상태에서 접근 방지
    if (m_frequency == 0)
    {
        OutputDebugString(L"[TimerClass] m_frequency is 0. Did you forget Initialize()?\n");
        return 0.0;
    }

    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);

    double delta = double(current.QuadPart - m_lastTime) / double(m_frequency);
    m_lastTime = current.QuadPart;

    return delta;
}
