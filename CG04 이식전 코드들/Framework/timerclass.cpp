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
    // QueryPerformanceFrequency �� ī���� ���ļ��� ���ؼ� m_frequency �� ����
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));

    // QueryPerformanceCounter �� ���� ī���� ���� m_lastTime �� ����
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_lastTime));
}

double TimerClass::GetDeltaTime()
{
    // ���: �ʱ�ȭ �� �� ���¿��� ���� ����
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
