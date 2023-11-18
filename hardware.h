#pragma once
#include "cpu.h"
#include "ram.h"
#include "gpu.h"

struct RamInfo
{
    int total;
    int used;
    int load;
};

struct GpuInfo
{
    RamInfo ramInfo;
    int load;
    int temperature;
};

struct CpuInfo
{
    int temperature;
    int load;
};
class Hardware {
public:
    Hardware();
    ~Hardware();
	CpuInfo getCpuInfo();
    GpuInfo getGpuInfo();
    RamInfo getRamInfo();
private:
    Cpu* cpu;
    Ram* ram;
    Gpu* gpu;
};