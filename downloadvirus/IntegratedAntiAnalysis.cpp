#include "IntegratedAntiAnalysis.h"
#include "Environmental_testing.h"   // 磁盘、CPU 检测
#include "AntiSandbox.h"             // 时间加速检测
#include "AntiDebug.h"               // 硬件断点、时间戳异常检测

// ==================== 构造 / 析构 ====================
IntegratedAntiAnalysis::IntegratedAntiAnalysis() : m_dwTotalScore(0) {
    InitializeCriticalSection(&m_cs);
}

IntegratedAntiAnalysis::~IntegratedAntiAnalysis() {
    DeleteCriticalSection(&m_cs);
}

// ==================== 分数操作（线程安全） ====================
void IntegratedAntiAnalysis::AddScore(DWORD points) {
    EnterCriticalSection(&m_cs);
    m_dwTotalScore += points;
    LeaveCriticalSection(&m_cs);
}

DWORD IntegratedAntiAnalysis::GetScore() {
    EnterCriticalSection(&m_cs);
    DWORD ret = m_dwTotalScore;
    LeaveCriticalSection(&m_cs);
    return ret;
}

// ==================== 各检测线程实现 ====================
DWORD WINAPI IntegratedAntiAnalysis::DiskCheckThread(LPVOID lpParam) {
    IntegratedAntiAnalysis* self = static_cast<IntegratedAntiAnalysis*>(lpParam);
    Environmental_testing::DiskInfo info;
    const unsigned long long limit = 128ULL * 1024 * 1024 * 1024; // 128 GB

    if (Environmental_testing::GetDiskInfo(L"C:\\", info)) {
        if (info.totalBytes <= limit) {
            self->AddScore(2);   // 磁盘太小 -> 怀疑是沙箱
        }
    }
    else {
        self->AddScore(1);       // 获取失败 -> 轻度可疑
    }
    return 0;
}

DWORD WINAPI IntegratedAntiAnalysis::CpuCheckThread(LPVOID lpParam) {
    IntegratedAntiAnalysis* self = static_cast<IntegratedAntiAnalysis*>(lpParam);
    if (Environmental_testing::GetCpuCoreCount() < 8) {
        self->AddScore(2);       // 核心数过少 -> 低配分析机
    }
    return 0;
}

DWORD WINAPI IntegratedAntiAnalysis::TimeAccelCheckThread(LPVOID lpParam) {
    IntegratedAntiAnalysis* self = static_cast<IntegratedAntiAnalysis*>(lpParam);
    if (AntiSandbox::DetectTimeAcceleration_GetTickCount()) {
        self->AddScore(2);       // 时间被加速 -> 沙箱特征
    }
    return 0;
}

DWORD WINAPI IntegratedAntiAnalysis::HardwareBpCheckThread(LPVOID lpParam) {
    IntegratedAntiAnalysis* self = static_cast<IntegratedAntiAnalysis*>(lpParam);
    if (AntiDebug::DetectHardwareBreakpoints()) {
        self->AddScore(3);       // 硬件断点 -> 极强调试信号
    }
    return 0;
}

DWORD WINAPI IntegratedAntiAnalysis::TimingAnomalyCheckThread(LPVOID lpParam) {
    IntegratedAntiAnalysis* self = static_cast<IntegratedAntiAnalysis*>(lpParam);
    if (AntiDebug::DetectTimingAnomaly()) {
        self->AddScore(1);       // 指令执行延迟 -> 轻度可疑
    }
    return 0;
}

// ==================== 主评估入口 ====================
BOOL IntegratedAntiAnalysis::Evaluate(DWORD dwTimeoutMs, DWORD dwThreshold) {
    const int THREAD_COUNT = 5;
    HANDLE hThreads[THREAD_COUNT] = { NULL };

    // 创建所有检测线程
    hThreads[0] = CreateThread(NULL, 0, DiskCheckThread, this, 0, NULL);
    hThreads[1] = CreateThread(NULL, 0, CpuCheckThread, this, 0, NULL);
    hThreads[2] = CreateThread(NULL, 0, TimeAccelCheckThread, this, 0, NULL);
    hThreads[3] = CreateThread(NULL, 0, HardwareBpCheckThread, this, 0, NULL);
    hThreads[4] = CreateThread(NULL, 0, TimingAnomalyCheckThread, this, 0, NULL);

    // 任一线程创建失败则立即判定不安全
    for (int i = 0; i < THREAD_COUNT; i++) {
        if (hThreads[i] == NULL) {
            // 关闭已创建的线程句柄
            for (int j = 0; j < i; j++) CloseHandle(hThreads[j]);
            return FALSE;
        }
    }

    // 轮询评分，直到超时或超标
    DWORD dwStart = GetTickCount64();
    while (TRUE) {
        if (GetScore() >= dwThreshold) {
            // 分数超标，强制结束所有检测线程并返回不安全
            for (int i = 0; i < THREAD_COUNT; i++) {
                TerminateThread(hThreads[i], 0);
                CloseHandle(hThreads[i]);
            }
            return FALSE;
        }
        if (GetTickCount64() - dwStart > dwTimeoutMs) {
            break;  // 超时，退出轮询
        }
        Sleep(200);
    }

    // 等待所有线程自然退出（可能还有线程未完成）
    WaitForMultipleObjects(THREAD_COUNT, hThreads, TRUE, INFINITE);
    for (int i = 0; i < THREAD_COUNT; i++) CloseHandle(hThreads[i]);

    // 最终确认分数
    return (GetScore() < dwThreshold);
}