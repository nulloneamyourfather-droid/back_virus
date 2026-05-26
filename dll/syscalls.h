#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <windows.h>
// NTSTATUS 已由 Windows 头文件定义，无需再定义

#ifdef __cplusplus
extern "C" {
#endif

    // 供汇编使用的全局变量
    extern DWORD wNtWaitForSingleObject;
    extern UINT_PTR sysAddrNtWaitForSingleObject;

    // 汇编实现的回调
    extern VOID CALLBACK WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);
    extern VOID CALLBACK Work2Callback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);
    extern VOID CALLBACK Work3Callback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);
    extern VOID CALLBACK Work4Callback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);

    // 汇编实现的 NtWaitForSingleObject（返回 NTSTATUS）
    extern NTSTATUS NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout);

    // 参数结构体
    typedef struct _NTALLOCATEVIRTUALMEMORY_ARGS {
        UINT_PTR pNtAllocateVirtualMemory;
        HANDLE hProcess;
        PVOID* address;
        PSIZE_T size;
        ULONG permissions;
    } NTALLOCATEVIRTUALMEMORY_ARGS;

    typedef struct _NTWRITEVIRTUALMEMORY_ARGS {
        UINT_PTR pNtWriteVirtualMemory;
        HANDLE hProcess;
        LPVOID baseAddress;
        LPVOID buffer;
        SIZE_T size;
        PSIZE_T bytesWritten;
    } NTWRITEVIRTUALMEMORY_ARGS;

    typedef struct _NTCREATETHREADEX_ARGS {
        UINT_PTR pNtCreateThreadEx;
        PHANDLE hThread;
        ACCESS_MASK access;
        PVOID objectAttributes;
        HANDLE hProcess;
        PVOID startRoutine;
        PVOID argument;
        ULONG flags;
        SIZE_T zeroBits;
        SIZE_T stackSize;
        SIZE_T maxStackSize;
        PVOID attributeList;
    } NTCREATETHREADEX_ARGS;

    typedef struct _NTPROTECTVIRTUALMEMORY_ARGS {
        UINT_PTR pNtProtectVirtualMemory;
        HANDLE hProcess;
        PVOID* address;
        PSIZE_T size;
        ULONG newProtection;
        PULONG oldProtection;
    } NTPROTECTVIRTUALMEMORY_ARGS;

#ifdef __cplusplus
}
#endif

#endif
