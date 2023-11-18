#include "serialPorts.h"

HANDLE hSerial;

void SerialPorts::close() {
	try
	{
		if (hSerial != NULL && hSerial != INVALID_HANDLE_VALUE) {
			CloseHandle(hSerial);
			hSerial = NULL; // 避免悬挂引用
		}
		isLink = false;
	}
	catch (const std::exception&)
	{
		throw std::invalid_argument("关闭串口失败！");
	}
}

SerialPorts::SerialPorts()
{
	isLink = false;
	try
	{
		SerialPorts::linkSreialPorts();
	}
	catch (const std::exception&)
	{
		throw std::invalid_argument("连接串口失败！");
	}
}

SerialPorts::~SerialPorts()
{
}

std::vector<std::wstring> SerialPorts::enumSerialPorts() {
	std::vector<std::wstring> ports;
	HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (deviceInfoSet == INVALID_HANDLE_VALUE) {
		throw std::invalid_argument("获取串口失败！");
		return ports;
	}

	SP_DEVINFO_DATA devInfoData;
	devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_COMPORT, i, &deviceInterfaceData); i++) {
		DWORD requiredSize = 0;
		SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

		PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
		if (deviceInterfaceDetailData) {
			deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceInterfaceDetailData, requiredSize, NULL, &devInfoData)) {
				ports.push_back(deviceInterfaceDetailData->DevicePath);
			}

			free(deviceInterfaceDetailData);
		}
	}

	SetupDiDestroyDeviceInfoList(deviceInfoSet);
	return ports;
}

uint32_t SerialPorts::encode(uint32_t data) {
	uint32_t dataTemp = data, dataTemp8;
	for (uint8_t i = 0; i < sizeof(data) -1; i++)
	{
		dataTemp = dataTemp << 8;
		dataTemp8 = data << ((2 - i) * 8);
		dataTemp8 = dataTemp8 >> (3 * 8);
		dataTemp = dataTemp + dataTemp8;
	}
	return dataTemp;
}

void SerialPorts::linkSreialPorts() {
	try
	{
		std::vector<std::wstring> serialPorts = enumSerialPorts();
		for (const auto& port : serialPorts) {
			// 打开串口
			hSerial = CreateFile(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hSerial == INVALID_HANDLE_VALUE) {
				std::cerr << "Error opening serial port" << std::endl;
				continue;
			}

			// 设置超时
			COMMTIMEOUTS timeouts = { 0 };
			timeouts.ReadIntervalTimeout = MAXDWORD; // 读操作之间的最大时间间隔
			timeouts.ReadTotalTimeoutConstant = 500;  // 读操作的总超时时间
			timeouts.ReadTotalTimeoutMultiplier = 100; // 每个字节的超时时间
			if (!SetCommTimeouts(hSerial, &timeouts)) {
				std::cerr << "Error setting timeouts" << std::endl;
				SerialPorts::close();
				continue;
			}

			// 设置串口参数
			DCB dcbSerialParams = { 0 };
			dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
			if (!GetCommState(hSerial, &dcbSerialParams)) {
				std::cerr << "Error getting serial port state" << std::endl;
				SerialPorts::close();
				continue;
			}
			dcbSerialParams.BaudRate = CBR_9600;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;
			dcbSerialParams.fParity = false;
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;
			if (!SetCommState(hSerial, &dcbSerialParams)) {
				std::cerr << "Error setting serial port state" << std::endl;
				SerialPorts::close();
				continue;
			}

			// 发送握手数据
			uint32_t data = encode(0x5aa5ff01);
			DWORD bytesWritten;
			if (!WriteFile(hSerial,&data, sizeof(data), &bytesWritten, NULL)) {
				std::cerr << "Error writing to serial port" << std::endl;
				SerialPorts::close();
				continue;
			}

			data = 0x0;

			// 读取串口
			DWORD bytesRead;
			if (!ReadFile(hSerial, &data, sizeof(data), &bytesRead, NULL)) {
				std::cerr << "Error reading from serial port" << std::endl;
				SerialPorts::close();
				continue;
			}

			data = encode(data);

			std::cout << "Received: " << data << std::endl;

			if (data == 0x5AA5FF10) {
				isLink = true;
				break;
			}
			else {
				SerialPorts::close();
				continue;
			}

		}
	}
	catch (const std::exception&)
	{
		throw std::invalid_argument("获取串口失败！");
	}

}

void SerialPorts::writeData(uint32_t cmd, uint8_t data) {
	if (isLink){
		uint32_t dataTemp = cmd << 8;
		dataTemp = dataTemp + data;
		dataTemp = encode(dataTemp);
		if (!WriteFile(hSerial, &dataTemp, sizeof(dataTemp), NULL, NULL)) {
			std::cerr << "Error writing to serial port" << std::endl;
			SerialPorts::close();
		}

		if (!ReadFile(hSerial, &dataTemp, sizeof(dataTemp), NULL, NULL)) {
			std::cerr << "Error reading from serial port" << std::endl;
			SerialPorts::close();
		}

		dataTemp = encode(dataTemp);

		std::cout << "Received: " << dataTemp << std::endl;

		if (dataTemp != ((cmd << 8) + 0xff)) {
			SerialPorts::close();
		}
	}
	else {
		try
		{
			SerialPorts::linkSreialPorts();
		}
		catch (const std::exception&)
		{
			throw std::invalid_argument("连接串口失败！");
		}
	}
}