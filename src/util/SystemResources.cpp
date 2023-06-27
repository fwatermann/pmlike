//
// Created by finnw on 04.06.2023.

// Windows implementation by https://stackoverflow.com/users/7381/lanzelot @ (https://stackoverflow.com/questions/63166)
//

#include "SystemResources.hpp"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif
#ifdef __linux__
#include <unistd.h>
#endif

using namespace pmlike::util;

size_t os::getTotalVirtualMemory() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo.ullTotalPageFile;
#else
    return 0;
#endif
}

size_t os::getUsedVirtualMemory() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG physMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    return physMemUsed;
#else
    return 0;
#endif
}

size_t os::getTotalPhysicalMemory() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo.ullTotalPhys;
#else
    return 0;
#endif
}

size_t os::getUsedPhysicalMemory() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    return physMemUsed;
#else
    return 0;
#endif
}

size_t os::getUsedVirtualMemoryOfCurrentProcess() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.PrivateUsage;
#else
    return 0;
#endif
}

size_t os::getUsedPhysicalMemoryOfCurrentProcess() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.WorkingSetSize;
#else
    return 0;
#endif
}

void os::initCPU() {
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    FILETIME ftime, fsys, fuser;

    GetSystemInfo(&sysinfo);
    numProcessors = sysinfo.dwNumberOfProcessors;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));

    self = GetCurrentProcess();
    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
#else
    return;
#endif
}

double os::getCPUUsageOfCurrentProcess() {
#ifdef _WIN32
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    float percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));
    percent = (sys.QuadPart - lastSysCPU.QuadPart) +
              (user.QuadPart - lastUserCPU.QuadPart);
    percent /= (now.QuadPart - lastCPU.QuadPart);
    percent /= numProcessors;
    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;

    return percent * 100.0f;
#else
    return -1;
#endif
}
