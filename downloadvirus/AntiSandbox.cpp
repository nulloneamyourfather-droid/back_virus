#include "AntiSandbox.h"
#include <iostream>
#include <vector>
#include <string>

// =====================================================================
// 技术一：GetTickCount 时间加速检测
// =====================================================================

BOOL AntiSandbox::DetectTimeAcceleration_GetTickCount()
{
    // 1. 记录 Sleep 前的时间戳
    DWORD dwStart = GetTickCount64();

    // 2. 执行一个较长时间的 Sleep（例如 5000 毫秒）
    //    沙箱为了快速完成分析，通常会 Hook Sleep 函数，大幅缩短实际等待时间
    Sleep(5000);

    // 3. 记录 Sleep 后的时间戳
    DWORD dwEnd = GetTickCount64();

    // 4. 计算实际经过的时间
    DWORD dwElapsed = dwEnd - dwStart;

    // 5. 判断：如果实际耗时远小于预期的 5000ms（例如小于 4500ms）
    //    则说明时间被加速了，很可能处于沙箱环境中
    //    安全边际设为 4500ms，既容忍正常系统的轻微误差，又能检测出沙箱的加速行为
    if (dwElapsed < 4500) {
     ///*   std::cout << "[!] 时间加速检测：疑似沙箱环境（实际耗时 "
     //       << dwElapsed << "ms < 预期 4500ms）" << std::endl;*/
        return TRUE;
    }

 /*   std::cout << "[*] 时间加速检测：环境正常（实际耗时 "
        //<< dwElapsed << "ms）" << std::endl;*/
    return FALSE;
}


// =====================================================================
// 技术二：API 洪流 (API Flooding)
// =====================================================================

void AntiSandbox::SpamGarbageAPIs(DWORD iterations)
{
    for (DWORD i = 0; i < iterations; i++) {
        // 构造一个临时字符串用于垃圾操作
        std::string tempStr = "sandbox_flood_data_" + std::to_string(i);

        // 获取系统目录路径（产生文件系统 API 调用日志）
        CHAR sysPath[MAX_PATH] = { 0 };
        GetSystemDirectoryA(sysPath, MAX_PATH);

        // 获取 Windows 目录（产生额外的 API 调用日志）
        CHAR winPath[MAX_PATH] = { 0 };
        GetWindowsDirectoryA(winPath, MAX_PATH);

        // 获取计算机名称（产生网络相关信息查询日志）
        DWORD dwSize = MAX_COMPUTERNAME_LENGTH + 1;
        CHAR computerName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
        GetComputerNameA(computerName, &dwSize);

        // 获取系统度量信息（产生大量信息查询日志）
        GetSystemMetrics(SM_CXSCREEN);
        GetSystemMetrics(SM_CYSCREEN);

        // 查询当前进程句柄（产生进程相关日志）
        HANDLE hProcess = GetCurrentProcess();
        if (hProcess) {
            SetHandleInformation(hProcess, HANDLE_FLAG_PROTECT_FROM_CLOSE,
                HANDLE_FLAG_PROTECT_FROM_CLOSE);
        }

        // 分配并释放内存（产生内存操作日志）
        LPVOID pMem = VirtualAlloc(NULL, 4096, MEM_COMMIT, PAGE_READWRITE);
        if (pMem) {
            VirtualFree(pMem, 0, MEM_RELEASE);
        }
    }
}

void AntiSandbox::APIFlooding_Delay()
{
    /*std::cout << "[*] 开始执行 API 洪流攻击..." << std::endl;*/

    // 执行大量垃圾 API 调用，以淹没沙箱日志
    // 实际恶意软件中可能会调用数十万甚至上百万次（如 Nymaim 变种曾调用超 50 万次）
    const DWORD TOTAL_ITERATIONS = 100000;  // 10 万次调用

    // 分批次执行，每批之间加入微小延迟，使沙箱更难通过简单跳过循环来规避
    const DWORD BATCH_SIZE = 10000;
    const DWORD NUM_BATCHES = TOTAL_ITERATIONS / BATCH_SIZE;

    for (DWORD batch = 0; batch < NUM_BATCHES; batch++) {
        SpamGarbageAPIs(BATCH_SIZE);

        // 微小延迟，防止 CPU 占用过高引起沙箱的特殊标记
        Sleep(10);
    }

    ////std::cout << "[*] API 洪流执行完毕（共 " << TOTAL_ITERATIONS
    ////    << " 次 API 调用）" << std::endl;
}