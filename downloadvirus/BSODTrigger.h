#pragma once

#include <Windows.h>
#include <winternl.h>
#include <stdexcept>

class BSODTrigger {
public:
    BSODTrigger();
    ~BSODTrigger();

    // 触发蓝屏（通常立即关机/崩溃）
    bool TriggerBSOD();

private:
    // 函数指针类型定义
    typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(
        ULONG Privilege,
        BOOLEAN Enable,
        BOOLEAN CurrentThread,
        PBOOLEAN Enabled
        );
    typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(
        NTSTATUS ErrorStatus,
        ULONG NumberOfParameters,
        ULONG UnicodeStringParameterMask OPTIONAL,
        PULONG_PTR Parameters,
        ULONG ResponseOption,
        PULONG Response
        );

    HMODULE m_hNtdll;
    pdef_RtlAdjustPrivilege m_pfnRtlAdjustPrivilege;
    pdef_NtRaiseHardError   m_pfnNtRaiseHardError;
    BOOLEAN                 m_bEnabled;
    ULONG                   m_uResp;
};