#pragma once

#include <windows.h>

class CPUClass
{
public:
    CPUClass();
    ~CPUClass();

    // 초기화 → 첫 번째 시스템 타임스탬프 가져오기
    void Initialize();

    // 한 프레임마다 호출, 내부에서 CPU 사용률 계산
    void Frame();

    // 계산된 CPU 사용률 반환 (0~100 사이)
    float GetCPUPercent() const;

private:
    // 이전 GetSystemTimes 정보
    ULONGLONG m_idleTimeOld;
    ULONGLONG m_kernelTimeOld;
    ULONGLONG m_userTimeOld;

    float m_cpuPercent;

    // FILETIME → 64비트 정수
    ULONGLONG FileTimeToUInt64(const FILETIME& ft) const;
};
