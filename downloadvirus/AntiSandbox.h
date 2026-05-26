#pragma once

#include <Windows.h>

///**
// * 反沙箱检测类
// * 提供基于时间加速检测和 API 洪流的沙箱逃逸技术
// */
class AntiSandbox {
public:
    // ---- 技术一：GetTickCount 时间加速检测 ----

    ///**
    // * 检测沙箱是否通过 Hook 加速了时间流逝
    // * 原理：测量 Sleep(5000ms) 的实际耗时，若实际耗时远小于预期，说明沙箱快进了时间。
    // * @return TRUE 表示可能运行在沙箱中（时间被加速），FALSE 表示环境正常
    // */
    static BOOL DetectTimeAcceleration_GetTickCount();

    // ---- 技术二：API 洪流 (API Flooding) ----

    ///**
    // * 通过大量垃圾 API 调用来淹没沙箱日志，使其达到分析上限
    // * 典型应用：在真实恶意逻辑执行前调用，可耗尽沙箱日志缓冲区或超过分析时间窗口
    // */
    static void APIFlooding_Delay();

private:
    ///**
    // * 单次垃圾 API 调用循环
    // * @param iterations 要执行的垃圾 API 调用次数
    // */
    static void SpamGarbageAPIs(DWORD iterations);
};