#pragma once

#include "timerclass.h"

class FPSClass
{
public:
    FPSClass();
    ~FPSClass();

    // 타이머 포인터를 받아 초기화
    void Initialize(TimerClass* timer);

    // 한 프레임이 끝날 때마다 호출, 내부에서 1초마다 FPS 계산
    void Frame(double deltaTime);

    // 현재 계산된 FPS 값 반환
    int GetFPS() const;

private:
    TimerClass* m_timer;
    int m_frameCount;      // 누적된 프레임 수
    int m_fps;             // 마지막으로 계산된 FPS
    double m_timeElapsed;  // 마지막으로 FPS를 계산한 이후 누적된 시간
};
