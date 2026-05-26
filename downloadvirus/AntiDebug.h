#pragma once
#include <Windows.h>

class AntiDebug {
public:
    // 时间戳反调试（内联汇编版，仅 x86；x64 请使用下方备选方案）
    static BOOL DetectTimingAnomaly();

    // 硬件断点检测（通用）
    static BOOL DetectHardwareBreakpoints();
};