#pragma once
#include <stdexcept>
#include "nvml.h"
#pragma comment(lib, "nvml.lib")
#include <Windows.h>
enum GpuVendor
{
	gpu_nvidia,
	gpu_amd,
	gpu_unknown
};
class Gpu
{
public:
	Gpu();
	~Gpu();
	int getGpuLoad();
	int getGpuTemperature();
	int getGpuRamLoad();
	int getGpuRamUsed();
	int getGpuRamTotal();

private:
	nvmlReturn_t result;
	HMODULE nvmlLibrary = NULL;
	void upDate();
	int gpuLoad;
	int gpuTemperature;
	int gpuRamLoad;
	int gpuRamUsed;
	int gpuRamTotal;
	void nvidiaInit();
	void amdInit();
	GpuVendor gpuVendor;
};