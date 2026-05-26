#include "AntiDebug.h"
#include <intrin.h> 
BOOL AntiDebug::DetectTimingAnomaly()
{
    unsigned __int64 start = __rdtsc();

    // 执行一段简单计算
    volatile int result = 0;
    volatile int a = 1, b = 2;
    for (int i = 0; i < 10; i++) {
        result += a * b;
    }

    unsigned __int64 end = __rdtsc();

    // 根据 CPU 频率设定阈值（这里示例：大于 10,000 个周期算异常）
    // 实际需根据目标机频率调整，例如 3GHz 下 10ms ≈ 30,000,000 周期
    if ((end - start) > 100000) {   // 阈值可调
        return TRUE;
    }
    return FALSE;
}


// ==================== 硬件断点检测（通用） ====================
BOOL AntiDebug::DetectHardwareBreakpoints()
{
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if (!GetThreadContext(GetCurrentThread(), &ctx))
    {
        return FALSE;   // 获取失败，安全返回
    }

    if (ctx.Dr0 != 0 || ctx.Dr1 != 0 || ctx.Dr2 != 0 || ctx.Dr3 != 0)
    {
        return TRUE;    // 发现硬件断点
    }
    return FALSE;
}