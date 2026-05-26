#include "BSODTrigger.h"
#include <iostream>

BSODTrigger::BSODTrigger()
    : m_hNtdll(nullptr)
    , m_pfnRtlAdjustPrivilege(nullptr)
    , m_pfnNtRaiseHardError(nullptr)
    , m_bEnabled(FALSE)
    , m_uResp(0)
{
    // 加载 ntdll.dll（增加引用计数，析构时释放）
    m_hNtdll = LoadLibraryA("ntdll.dll");
    if (!m_hNtdll) {
        throw std::runtime_error("Failed to load ntdll.dll");
    }

    // 获取函数地址
    m_pfnRtlAdjustPrivilege = reinterpret_cast<pdef_RtlAdjustPrivilege>(
        GetProcAddress(m_hNtdll, "RtlAdjustPrivilege"));
    m_pfnNtRaiseHardError = reinterpret_cast<pdef_NtRaiseHardError>(
        GetProcAddress(m_hNtdll, "NtRaiseHardError"));

    if (!m_pfnRtlAdjustPrivilege || !m_pfnNtRaiseHardError) {
        FreeLibrary(m_hNtdll);
        m_hNtdll = nullptr;  // 避免析构函数再次释放
        throw std::runtime_error("Failed to get required function addresses");
    }
}

BSODTrigger::~BSODTrigger() {
    if (m_hNtdll) {
        FreeLibrary(m_hNtdll);
    }
}

bool BSODTrigger::TriggerBSOD() {
    // 1. 启用 SeShutdownPrivilege（特权 ID = 19）
    NTSTATUS status = m_pfnRtlAdjustPrivilege(19, TRUE, FALSE, &m_bEnabled);
    if (!NT_SUCCESS(status)) {
        std::cerr << "RtlAdjustPrivilege failed: 0x"
            << std::hex << status << std::endl;
        return false;
    }

    // 2. 引发硬件错误，响应选项 6 表示立即蓝屏/关机
    status = m_pfnNtRaiseHardError(
        STATUS_FLOAT_MULTIPLE_FAULTS,   // 错误状态码
        0,                              // 参数个数
        0,                              // 参数掩码
        nullptr,                        // 参数指针
        6,                              // ResponseOption (立即关机)
        &m_uResp                        // 返回的响应
    );

    // 通常执行不到这里，若返回则说明失败
    std::cerr << "NtRaiseHardError returned: 0x"
        << std::hex << status << std::endl;
    return false;
}