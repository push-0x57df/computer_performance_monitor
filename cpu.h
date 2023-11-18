#pragma once
#include<stdint.h>
#include<Windows.h>
#include"wmi.h"
// �洢CPU���̱��
enum CpuVendor
{
	intel,
	amd,
	unknown
};

class Cpu
{
public:
	Cpu(Wmi* wmi);
	int getCpuTemperature();
	int getCpuLoad();

private:
	int getIntelTjMax(DWORD processAffinityMask);
	int getIntelDelta(DWORD processAffinityMask);
	CpuVendor cpuVendor;
	uint16_t family;
	uint8_t model;
	Wmi* thisWmi;
};