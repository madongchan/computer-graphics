#include "cpuclass.h"

CPUClass::CPUClass()
    : m_idleTimeOld(0)
    , m_kernelTimeOld(0)
    , m_userTimeOld(0)
    , m_cpuPercent(0.0f)
{
}

CPUClass::~CPUClass()
{
}

ULONGLONG CPUClass::FileTimeToUInt64(const FILETIME& ft) const
{
    return (static_cast<ULONGLONG>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

void CPUClass::Initialize()
{
    FILETIME idleTime, kernelTime, userTime;
    ::GetSystemTimes(&idleTime, &kernelTime, &userTime);

    m_idleTimeOld = FileTimeToUInt64(idleTime);
    m_kernelTimeOld = FileTimeToUInt64(kernelTime);
    m_userTimeOld = FileTimeToUInt64(userTime);
}

void CPUClass::Frame()
{
    FILETIME idleTime, kernelTime, userTime;
    ::GetSystemTimes(&idleTime, &kernelTime, &userTime);

    ULONGLONG idleNew = FileTimeToUInt64(idleTime);
    ULONGLONG kernelNew = FileTimeToUInt64(kernelTime);
    ULONGLONG userNew = FileTimeToUInt64(userTime);

    ULONGLONG idleDiff = idleNew - m_idleTimeOld;
    ULONGLONG kernelDiff = kernelNew - m_kernelTimeOld;
    ULONGLONG userDiff = userNew - m_userTimeOld;

    // 실제 CPU 시간 = (kernelDiff + userDiff) - idleDiff
    ULONGLONG total = (kernelDiff + userDiff);
    ULONGLONG busy = total - idleDiff;

    if (total > 0)
        m_cpuPercent = (busy * 100.0f) / total;
    else
        m_cpuPercent = 0.0f;

    m_idleTimeOld = idleNew;
    m_kernelTimeOld = kernelNew;
    m_userTimeOld = userNew;
}

float CPUClass::GetCPUPercent() const
{
    return m_cpuPercent;
}
