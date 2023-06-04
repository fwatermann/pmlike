//
// Created by finnw on 04.06.2023.
//

#ifndef PM_LIKE_SYSTEMRESOURCES_HPP
#define PM_LIKE_SYSTEMRESOURCES_HPP

#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

namespace pmlike::util::os {
    static const size_t MB = 1024 * 1024;

#ifdef _WIN32
    static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    static int numProcessors;
    static HANDLE self;
#endif

    size_t getTotalVirtualMemory();
    size_t getUsedVirtualMemory();
    size_t getTotalPhysicalMemory();
    size_t getUsedPhysicalMemory();
    size_t getUsedVirtualMemoryOfCurrentProcess();
    size_t getUsedPhysicalMemoryOfCurrentProcess();

    void initCPU();
    double getCPUUsageOfCurrentProcess();

}


#endif //PM_LIKE_SYSTEMRESOURCES_HPP
