#pragma once

#include <windows.h>

class TimerClass
{
public:
    TimerClass();
    ~TimerClass();

    // 프로그램 시작 시 한 번 호출
    void Initialize();

    // 매 프레임 호출 → 이전 프레임 대비 델타 타임(초)을 반환
    double GetDeltaTime();

private:
    __int64 m_frequency;
    __int64 m_lastTime;
};
