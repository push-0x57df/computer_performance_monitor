#include "gpu.h"
// ���庯��ָ������
typedef nvmlReturn_t(*nvmlInit_t)();
typedef nvmlReturn_t(*nvmlShutdown_t)();
typedef nvmlReturn_t(*nvmlDeviceGetCount_t)(unsigned int* deviceCount);
typedef nvmlReturn_t(*nvmlDeviceGetHandleByIndex_t)(unsigned int index, nvmlDevice_t* device);
typedef nvmlReturn_t(*nvmlDeviceGetTemperature_t)(nvmlDevice_t device, nvmlTemperatureSensors_t sensorType, unsigned int* temp);
typedef nvmlReturn_t(*nvmlDeviceGetMemoryInfo_t)(nvmlDevice_t device, nvmlMemory_t* memory);
typedef nvmlReturn_t(*nvmlDeviceGetUtilizationRates_t)(nvmlDevice_t device, nvmlUtilization_t* utilization);

// ����ȫ�ֺ���ָ��
nvmlInit_t nvmlInit_f = NULL;
nvmlShutdown_t nvmlShutdown_f = NULL;
nvmlDeviceGetCount_t nvmlDeviceGetCount_f = NULL;
nvmlDeviceGetHandleByIndex_t nvmlDeviceGetHandleByIndex_f = NULL;
nvmlDeviceGetTemperature_t nvmlDeviceGetTemperature_f = NULL;
nvmlDeviceGetMemoryInfo_t nvmlDeviceGetMemoryInfo_f = NULL;
nvmlDeviceGetUtilizationRates_t nvmlDeviceGetUtilizationRates_f = NULL;

void Gpu::nvidiaInit() {

	// ���� nvml.dll
	nvmlLibrary = LoadLibraryA("nvml.dll");
	if (nvmlLibrary == NULL) {
		// std::cout << "Failed to load NVML library." << std::endl;
		throw std::invalid_argument("����Ӣΰ���Կ�����dllʧ��");
	}

	// ���غ���
	nvmlInit_f = (nvmlInit_t)GetProcAddress(nvmlLibrary, "nvmlInit_v2");
	nvmlShutdown_f = (nvmlShutdown_t)GetProcAddress(nvmlLibrary, "nvmlShutdown");
	nvmlDeviceGetCount_f = (nvmlDeviceGetCount_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetCount_v2");
	nvmlDeviceGetHandleByIndex_f = (nvmlDeviceGetHandleByIndex_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetHandleByIndex_v2");
	nvmlDeviceGetTemperature_f = (nvmlDeviceGetTemperature_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetTemperature");
	nvmlDeviceGetMemoryInfo_f = (nvmlDeviceGetMemoryInfo_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetMemoryInfo");
	nvmlDeviceGetUtilizationRates_f = (nvmlDeviceGetUtilizationRates_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetUtilizationRates");
	

	// �ж��Ƿ���سɹ�
	if (!nvmlInit_f || !nvmlShutdown_f || !nvmlDeviceGetCount_f || !nvmlDeviceGetHandleByIndex_f || !nvmlDeviceGetTemperature_f || !nvmlDeviceGetMemoryInfo_f || !nvmlDeviceGetUtilizationRates_f) {
		throw std::invalid_argument("����Ӣΰ���Կ�����dllʧ��");
	}

	result = nvmlInit_f();
	if (result != NVML_SUCCESS) {
		// std::cout << "Failed to initialize NVML: " << nvmlErrorString(result) << std::endl;
		throw std::invalid_argument("����Ӣΰ���Կ�����dllʧ��");
	}
	gpuVendor = gpu_nvidia;
}

void Gpu::amdInit() {
	throw std::invalid_argument("��ʱ�޷����AMD�Կ�");
}

Gpu::Gpu()
{
	gpuLoad = 0;
	gpuTemperature = 0;
	gpuRamLoad = 0;
	gpuRamTotal = 0;
	gpuRamUsed = 0;
	gpuVendor = gpu_unknown;
	try
	{
		nvidiaInit();
	}
	catch (const std::exception& e)
	{
		try
		{
			amdInit();
			gpuVendor = gpu_amd;
		}
		catch (const std::exception&e2)
		{
			std::string err = e.what();
			err += " ";
			err += e2.what();
			//throw std::invalid_argument(err);
		}
	}
}

Gpu::~Gpu()
{
	switch (gpuVendor)
	{
	case gpu_nvidia:
		FreeLibrary(nvmlLibrary);
		nvmlShutdown_f();
		break;
	case gpu_amd:
		break;
	case gpu_unknown:
		break;
	default:
		break;
	}
}

void Gpu::upDate() {
	switch (gpuVendor)
	{
	case gpu_nvidia:
		unsigned int deviceCount;
		result = nvmlDeviceGetCount_f(&deviceCount);
		if (result != NVML_SUCCESS) {
			// std::cout << "Failed to get device count: " << nvmlErrorString(result) << std::endl;
			throw std::invalid_argument("�޷���ȡӢΰ���Կ�����");
			FreeLibrary(nvmlLibrary);
			nvmlShutdown_f();
		}

		if (deviceCount > 0) {
			// ע�⣺Ӣΰ���������ܷ��ض���ʶ�𵽵��Կ����������������һ��̨ʽ���õ��ԣ�ֻʶ��һ�š�����i��ֵΪ���ص��Կ���ʶ
			int i = 0;
//
			nvmlDevice_t device;
//			char name[NVML_DEVICE_NAME_BUFFER_SIZE];
//
			result = nvmlDeviceGetHandleByIndex_f(i, &device);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get device handle: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("�޷���ȡӢΰ���Կ��������");
			}
//			
//			result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
//			if (result != NVML_SUCCESS) {
//				// std::cout << "Failed to get device name: " << nvmlErrorString(result) << std::endl;
//				throw std::invalid_argument("Failed to get device name.");
//			}
//			
			nvmlTemperatureSensors_t sensorType = NVML_TEMPERATURE_GPU;
			unsigned int temperature;
			result = nvmlDeviceGetTemperature_f(device, sensorType, &temperature);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get temperature: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("�޷���ȡӢΰ���Կ��¶�");
			}

			nvmlMemory_t memory;
			result = nvmlDeviceGetMemoryInfo_f(device, &memory);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get memory info: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("�޷���ȡӢΰ���Կ��ڴ���Ϣ");
			}

			nvmlUtilization_t utilization;
			result = nvmlDeviceGetUtilizationRates_f(device, &utilization);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get utilization rates: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("�޷���ȡӢΰ���Կ�������");
			}

			gpuRamTotal = uint8_t(memory.total / 1024 / 1024 / 1024);
			gpuRamUsed = uint8_t(memory.used / 1024 / 1024 / 1024);
			gpuRamLoad = uint8_t(memory.used * 100 / memory.total);
			gpuLoad = uint8_t(utilization.gpu);
			gpuTemperature = uint8_t(temperature);
		}
		break;
	case gpu_amd:
		break;
	case gpu_unknown:
		break;
	default:
		break;
	}

}

int Gpu::getGpuLoad() {
	try
	{
		upDate();
	}
	catch (const std::exception&e)
	{
		throw std::invalid_argument(e.what());
	}
	return gpuLoad;
}

int Gpu::getGpuTemperature() {
	try
	{
		upDate();
	}
	catch (const std::exception&e)
	{
		throw std::invalid_argument(e.what());
	}
	return gpuTemperature;
}

int Gpu::getGpuRamLoad() {
	try
	{
		upDate();
	}
	catch (const std::exception&e)
	{
		throw std::invalid_argument(e.what());
	}
	return gpuRamLoad;
}

int Gpu::getGpuRamTotal() {
	try
	{
		upDate();
	}
	catch (const std::exception&e)
	{
		throw std::invalid_argument(e.what());
	}
	return gpuRamTotal;
}

int Gpu::getGpuRamUsed() {
	try
	{
		upDate();
	}
	catch (const std::exception&e)
	{
		throw std::invalid_argument(e.what());
	}
	return gpuRamUsed;
}