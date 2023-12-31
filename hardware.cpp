#include "hardware.h"
#include <stdexcept>

CpuInfo Hardware::getCpuInfo() {
	CpuInfo cpuInfo;
	cpuInfo.load = 0;
	cpuInfo.temperature = 0;
	try
	{
		cpuInfo.temperature = cpu->getCpuTemperature();
		cpuInfo.load = cpu->getCpuLoad();
	}
	catch (const std::exception&e)
	{
		throw std::invalid_argument(e.what());
	}

	return cpuInfo;
}

GpuInfo Hardware::getGpuInfo() {
	GpuInfo gpuInfo;
	gpuInfo.load = 0;
	gpuInfo.temperature = 0;
	gpuInfo.ramInfo.load = 0;
	gpuInfo.ramInfo.used = 0;
	gpuInfo.ramInfo.total = 0;
	try
	{
		gpuInfo.load = gpu->getGpuLoad();
		gpuInfo.temperature = gpu->getGpuTemperature();
		gpuInfo.ramInfo.load = gpu->getGpuRamLoad();
		gpuInfo.ramInfo.used = gpu->getGpuRamUsed();
		gpuInfo.ramInfo.total = gpu->getGpuRamTotal();
	}
	catch (const std::exception&e)
	{
		throw std::invalid_argument(e.what());
	}
	return gpuInfo;
}

RamInfo Hardware::getRamInfo() {
	RamInfo ramInfo;
	ramInfo.load = 0;
	ramInfo.used = 0;
	ramInfo.total = 0;
	try
	{
		ramInfo.load = ram->readLoad();
		ramInfo.total = ram->readTotal();
		ramInfo.used = ram->readUsed();
	}
	catch (const std::exception&e)
	{
		throw std::invalid_argument(e.what());
	}
	return ramInfo;
}

Hardware::Hardware() {
	try
	{
		cpu = new Cpu(new Wmi);
	}
	catch (const std::exception& e)
	{
		throw std::invalid_argument(e.what());
	}

	try
	{
		ram = new Ram;
	}
	catch (const std::exception& e)
	{
		throw std::invalid_argument(e.what());
	}

	try
	{
		gpu = new Gpu;
	}
	catch (const std::exception& e)
	{
		throw std::invalid_argument(e.what());
	}
}

Hardware::~Hardware() {}