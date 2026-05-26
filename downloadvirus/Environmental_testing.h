#pragma once
#include <Windows.h>
#include <string>
class Environmental_testing
{
public:
    struct DiskInfo
    {
        unsigned long long totalBytes;
        unsigned long long freeBytes;
        unsigned long long availableBytes;
    };

public:
    // 获取指定磁盘信息，例如 L"C:\\"
    static bool GetDiskInfo(const std::wstring& rootPath, DiskInfo& diskInfo);

    // 获取 CPU 逻辑核心数
    static DWORD GetCpuCoreCount();

    // 字节转换为 GB
    static double BytesToGB(unsigned long long bytes);
};

