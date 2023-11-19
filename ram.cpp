#include"ram.h"

Ram::Ram()
{
    load = 0;
    used = 0;
    total = 0;
    memoryStatus.dwLength = sizeof(memoryStatus);
}

Ram::~Ram()
{
}

void Ram::upData() {
    if (GlobalMemoryStatusEx(&memoryStatus)) {
        DWORDLONG totalPhysicalMemory = memoryStatus.ullTotalPhys;
        DWORDLONG availablePhysicalMemory = memoryStatus.ullAvailPhys;
        DWORD memoryLoad = memoryStatus.dwMemoryLoad;

        // 计算内存使用率
        double memoryUsage = 100.0 * (totalPhysicalMemory - availablePhysicalMemory) / totalPhysicalMemory;

        total = totalPhysicalMemory / (1024 * 1024 * 1024);
        used = total - availablePhysicalMemory / (1024 * 1024 * 1024);
        load = memoryLoad;
    }
    else {
        throw std::invalid_argument("读取内存信息失败！");
    }
}

int Ram::readLoad() {
    try
    {
        Ram::upData();
    }
    catch (const std::exception&e)
    {
        throw std::invalid_argument(e.what());
    }
    return load;
}

int Ram::readTotal() {
    try
    {
        Ram::upData();
    }
    catch (const std::exception&e)
    {
        throw std::invalid_argument(e.what());
    }
    return total;
}

int Ram::readUsed() {
    try
    {
        Ram::upData();
    }
    catch (const std::exception&e)
    {
        throw std::invalid_argument(e.what());
    }
    return used;
}