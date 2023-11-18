#pragma once
#include <stdexcept>
#include <Windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "setupapi.lib")

class SerialPorts
{
public:
	SerialPorts();
	~SerialPorts();
	void writeData(uint32_t cmd, uint8_t data);

private:
	std::vector<std::wstring> enumSerialPorts();
	void linkSreialPorts();
	bool isLink;
	void close();
	uint32_t encode(uint32_t data);
};