#include "Environmental_testing.h"
bool Environmental_testing::GetDiskInfo(const std::wstring& rootPath, DiskInfo& diskInfo)
{
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;

    BOOL result = GetDiskFreeSpaceExW(
        rootPath.c_str(),
        &freeBytesAvailable,
        &totalNumberOfBytes,
        &totalNumberOfFreeBytes
    );

    if (!result)
    {
        return false;
    }

    diskInfo.availableBytes = freeBytesAvailable.QuadPart;
    diskInfo.totalBytes = totalNumberOfBytes.QuadPart;
    diskInfo.freeBytes = totalNumberOfFreeBytes.QuadPart;

    return true;
}

DWORD Environmental_testing::GetCpuCoreCount()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}

double Environmental_testing::BytesToGB(unsigned long long bytes)
{
    return bytes / 1024.0 / 1024.0 / 1024.0;
}
