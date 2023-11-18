#pragma once
#include <stdexcept>
#include <Windows.h>
class  Ram
{
public:
	 Ram();
	~ Ram();
	int readLoad();
	int readTotal();
	int readUsed();

private:
	MEMORYSTATUSEX memoryStatus;
	int load;
	int total;
	int used;
	void upData();
};