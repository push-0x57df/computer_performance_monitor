#include "cpu.h"
#include "winring0.h"

Cpu::Cpu(Wmi* wmi) {
	thisWmi = wmi;
	cpuVendor = unknown;
	family = 0x0;
	model = 0x0;
	// 初始化winring0驱动
	try
	{
		WinRing0* winRing0 = new WinRing0;
	}
	catch (const std::exception& e)
	{

	}

	// 获取CPU生产商
	if (IsCpuid() == false)
	{
		throw std::invalid_argument("当前系统不支持CPUID指令！");
	}
	UCHAR vendorId[16];
	Cpuid(0, (PDWORD)&vendorId[12], (PDWORD)&vendorId[0], (PDWORD)&vendorId[8], (PDWORD)&vendorId[4]);
	std::string cpuVendorStr;
	cpuVendorStr = reinterpret_cast<char*>(vendorId);
	if (cpuVendorStr.find("GenuineIntel") != std::string::npos) {
		cpuVendor = intel;
	}
	else if (cpuVendorStr.find("AuthenticAMD") != std::string::npos) {
		cpuVendor = amd;
	}
	else {
		cpuVendor = unknown;
		throw std::invalid_argument("不支持的CPU生产商！");
	}

	// 获取CPU的系列号和型号
	DWORD eax, ebx, ecx, edx;
	Cpuid(0x1, &eax, &ebx, &ecx, &edx);
	family = (uint16_t)(eax >> 8) & 0xf;
	model = (uint16_t)(eax >> 4) & 0xf;
	if (family == 0xf || family == 0x6) {
		model = (((eax >> 16) & 0xf) << 4) + model;
	}
	if (family == 0xf) {
		family = ((eax >> 20) & 0xff) + family;
	}
}

int Cpu::getIntelTjMax(DWORD processAffinityMask) {
	DWORD eax, edx;
	RdmsrPx(0x1a2, &eax, &edx, processAffinityMask);
	eax &= 0x00FF0000;
	int tjMax = (eax >> 16) & 0xff;
	return tjMax;
}

int Cpu::getIntelDelta(DWORD processAffinityMask) {
	DWORD eax, edx;
	RdmsrPx(0x19c, &eax, &edx, processAffinityMask);
	eax &= 0x007F0000;
	int delta = (eax >> 16) & 0xff;
	return delta;
}

int Cpu::getCpuTemperature() {
	int temperature = 0;
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int processorsNumber = systemInfo.dwNumberOfProcessors;
	int temperatureAll = 0;
	if (processorsNumber == 0) {
		throw std::invalid_argument("读取CPU核心数失败！");
	}
	switch (cpuVendor)
	{
	case intel:
		// 识别CPU tjMax
		switch (family)
		{
		case 0x6:
			switch (model)
			{
			case 0x1A: // Intel Core i7 LGA1366 (45nm)
			case 0x1E: // Intel Core i5, i7 LGA1156 (45nm)
			case 0x1F: // Intel Core i5, i7 
			case 0x25: // Intel Core i3, i5, i7 LGA1156 (32nm)
			case 0x2C: // Intel Core i7 LGA1366 (32nm) 6 Core
			case 0x2E: // Intel Xeon Processor 7500 series (45nm)
			case 0x2F: // Intel Xeon Processor (32nm)
			case 0x2A: // Intel Core i5, i7 2xxx LGA1155 (32nm)
			case 0x2D: // Next Generation Intel Xeon, i7 3xxx LGA2011 (32nm)
			case 0x3A: // Intel Core i5, i7 3xxx LGA1155 (22nm)
			case 0x3E: // Intel Core i7 4xxx LGA2011 (22nm)
			case 0x3C: // Intel Core i5, i7 4xxx LGA1150 (22nm)              
			case 0x3F: // Intel Xeon E5-2600/1600 v3, Core i7-59xx
				// LGA2011-v3, Haswell-E (22nm)
			case 0x45: // Intel Core i5, i7 4xxxU (22nm)
			case 0x46:
			case 0x3D: // Intel Core M-5xxx (14nm)
			case 0x47: // Intel i5, i7 5xxx, Xeon E3-1200 v4 (14nm)
			case 0x4F: // Intel Xeon E5-26xx v4
			case 0x56: // Intel Xeon D-15xx
			case 0x36: // Intel Atom S1xxx, D2xxx, N2xxx (32nm)
			case 0x37: // Intel Atom E3xxx, Z3xxx (22nm)
			case 0x4A:
			case 0x4D: // Intel Atom C2xxx (22nm)
			case 0x5A:
			case 0x5D:
			case 0x4E:
			case 0x5E: // Intel Core i5, i7 6xxxx LGA1151 (14nm)
			case 0x55: // Intel Core i7, i9 7xxxx LGA2066 (14nm)
			case 0x4C:
			case 0x8E:
			case 0x9E: // Intel Core i5, i7 7xxxx (14nm)
			case 0x5C: // Intel Atom processors (Apollo Lake) (14nm)
			case 0x5F: // Intel Atom processors (Denverton) (14nm)
			case 0x7A: // Intel Atom processors (14nm)
			case 0x66: // Intel Core i3 8121U (10nm)
			case 0x7D: // Intel Core i3, i5, i7 10xxGx (10nm) 
			case 0x7E:
			case 0x6A: // Intel Xeon (10nm)
			case 0x6C:
			case 0xA5:
			case 0xA6: // Intel Core i3, i5, i7 10xxxU (14nm)
			case 0x86: // Intel Atom processors
			case 0x8C: // Intel processors (10nm++)
			case 0x8D:
			case 0xb7: // Intel Core i7 13700kf
				temperatureAll = 0;
				for (int i = 0; i < processorsNumber; i++)
				{
					int tjMax = Cpu::getIntelTjMax(1u << i);
					int delta = Cpu::getIntelDelta(1u << i);
					temperatureAll += tjMax - delta;
				}

				temperature = temperatureAll / processorsNumber;
				break;
			default:
				throw std::invalid_argument("不支持的CPU型号！");
				break;
			}
			break;
		default:
			throw std::invalid_argument("不支持的CPU家族！");
			break;
		}
		break;
	case amd:
		break;
	case unknown:
		break;
	default:
		break;
	}
	return temperature;
};

int Cpu::getCpuLoad() {
	try
	{
		return thisWmi->getCpuLoadForWmi();
	}
	catch (const std::exception& e)
	{
	}
	return 0;
}