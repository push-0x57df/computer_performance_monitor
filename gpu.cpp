#include "gpu.h"

void Gpu::nvidiaInit() {
	result = nvmlInit();
	if (result != NVML_SUCCESS) {
		// std::cout << "Failed to initialize NVML: " << nvmlErrorString(result) << std::endl;
		throw std::invalid_argument("Failed to initialize NVML");
	}

	// 加载 nvml.dll
	nvmlLibrary = LoadLibraryA("nvml.dll");
	if (nvmlLibrary == NULL) {
		// std::cout << "Failed to load NVML library." << std::endl;
		throw std::invalid_argument("Failed to load NVML library.");
		nvmlShutdown();
	}
}

void Gpu::amdInit() {
	throw std::invalid_argument("wait build.");
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
		gpuVendor = gpu_nvidia;
	}
	catch (const std::exception&)
	{
		try
		{
			amdInit();
			gpuVendor = gpu_amd;
		}
		catch (const std::exception&)
		{

		}
	}
}

Gpu::~Gpu()
{
	switch (gpuVendor)
	{
	case gpu_nvidia:
		FreeLibrary(nvmlLibrary);
		nvmlShutdown();
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
		result = nvmlDeviceGetCount(&deviceCount);
		if (result != NVML_SUCCESS) {
			// std::cout << "Failed to get device count: " << nvmlErrorString(result) << std::endl;
			throw std::invalid_argument("Failed to get device count.");
			FreeLibrary(nvmlLibrary);
			nvmlShutdown();
		}

		if (deviceCount > 0) {
			// 注意：英伟达驱动可能返回多张识别到的显卡。但本程序服务于一般台式家用电脑，只识别一张。变量i的值为返回的显卡标识
			int i = 0;

			nvmlDevice_t device;
			char name[NVML_DEVICE_NAME_BUFFER_SIZE];

			result = nvmlDeviceGetHandleByIndex_v2(i, &device);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get device handle: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("Failed to get device handle.");
			}
			
			result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get device name: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("Failed to get device name.");
			}
			
			nvmlTemperatureSensors_t sensorType = NVML_TEMPERATURE_GPU;
			unsigned int temperature;
			result = nvmlDeviceGetTemperature(device, sensorType, &temperature);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get temperature: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("Failed to get temperature.");
			}

			nvmlMemory_t memory;
			result = nvmlDeviceGetMemoryInfo(device, &memory);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get memory info: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("Failed to get memory info.");
			}

			nvmlUtilization_t utilization;
			result = nvmlDeviceGetUtilizationRates(device, &utilization);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get utilization rates: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("Failed to get utilization rates.");
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
	catch (const std::exception&)
	{

	}
	return gpuLoad;
}

int Gpu::getGpuTemperature() {
	try
	{
		upDate();
	}
	catch (const std::exception&)
	{

	}
	return gpuTemperature;
}

int Gpu::getGpuRamLoad() {
	try
	{
		upDate();
	}
	catch (const std::exception&)
	{

	}
	return gpuRamLoad;
}

int Gpu::getGpuRamTotal() {
	try
	{
		upDate();
	}
	catch (const std::exception&)
	{

	}
	return gpuRamTotal;
}

int Gpu::getGpuRamUsed() {
	try
	{
		upDate();
	}
	catch (const std::exception&)
	{

	}
	return gpuRamUsed;
}