#pragma once

#include <windows.h>

class CPUClass
{
public:
    CPUClass();
    ~CPUClass();

    // �ʱ�ȭ �� ù ��° �ý��� Ÿ�ӽ����� ��������
    void Initialize();

    // �� �����Ӹ��� ȣ��, ���ο��� CPU ���� ���
    void Frame();

    // ���� CPU ���� ��ȯ (0~100 ����)
    float GetCPUPercent() const;

private:
    // ���� GetSystemTimes ����
    ULONGLONG m_idleTimeOld;
    ULONGLONG m_kernelTimeOld;
    ULONGLONG m_userTimeOld;

    float m_cpuPercent;

    // FILETIME �� 64��Ʈ ����
    ULONGLONG FileTimeToUInt64(const FILETIME& ft) const;
};
