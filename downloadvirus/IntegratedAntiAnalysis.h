#pragma once
#include <Windows.h>

/**
 //* 整合反分析引擎
 //* 将磁盘/CPU/时间加速/硬件断点/时间戳异常等检测并行化，
 //* 通过累加评分判断环境是否安全。
 //*/
class IntegratedAntiAnalysis {
public:
    IntegratedAntiAnalysis();
    ~IntegratedAntiAnalysis();

    ///**
    // * 启动所有检测线程，轮询评分，超时或超标时返回
    // * @param dwTimeoutMs  最长等待时间(毫秒)
    // * @param dwThreshold  可疑分数阈值，达到即判为不安全
    // * @return TRUE 环境安全，FALSE 环境不安全
    // */
    BOOL Evaluate(DWORD dwTimeoutMs = 10000, DWORD dwThreshold = 3);

private:
    // 线程入口函数（静态，通过 lpParam 获取 this 指针）
    static DWORD WINAPI DiskCheckThread(LPVOID lpParam);
    static DWORD WINAPI CpuCheckThread(LPVOID lpParam);
    static DWORD WINAPI TimeAccelCheckThread(LPVOID lpParam);
    static DWORD WINAPI HardwareBpCheckThread(LPVOID lpParam);
    static DWORD WINAPI TimingAnomalyCheckThread(LPVOID lpParam);

    // 线程安全的分数操作
    void AddScore(DWORD points);
    DWORD GetScore();

    // 同步对象
    CRITICAL_SECTION m_cs;
    DWORD m_dwTotalScore;
};