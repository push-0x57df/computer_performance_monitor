#include "gpu.h"
// 定义函数指针类型
typedef nvmlReturn_t(*nvmlInit_t)();
typedef nvmlReturn_t(*nvmlShutdown_t)();
typedef nvmlReturn_t(*nvmlDeviceGetCount_t)(unsigned int* deviceCount);
typedef nvmlReturn_t(*nvmlDeviceGetHandleByIndex_t)(unsigned int index, nvmlDevice_t* device);
typedef nvmlReturn_t(*nvmlDeviceGetTemperature_t)(nvmlDevice_t device, nvmlTemperatureSensors_t sensorType, unsigned int* temp);
typedef nvmlReturn_t(*nvmlDeviceGetMemoryInfo_t)(nvmlDevice_t device, nvmlMemory_t* memory);
typedef nvmlReturn_t(*nvmlDeviceGetUtilizationRates_t)(nvmlDevice_t device, nvmlUtilization_t* utilization);

// 创建全局函数指针
nvmlInit_t nvmlInit_f = NULL;
nvmlShutdown_t nvmlShutdown_f = NULL;
nvmlDeviceGetCount_t nvmlDeviceGetCount_f = NULL;
nvmlDeviceGetHandleByIndex_t nvmlDeviceGetHandleByIndex_f = NULL;
nvmlDeviceGetTemperature_t nvmlDeviceGetTemperature_f = NULL;
nvmlDeviceGetMemoryInfo_t nvmlDeviceGetMemoryInfo_f = NULL;
nvmlDeviceGetUtilizationRates_t nvmlDeviceGetUtilizationRates_f = NULL;

void Gpu::nvidiaInit() {

	// 加载 nvml.dll
	nvmlLibrary = LoadLibraryA("nvml.dll");
	if (nvmlLibrary == NULL) {
		// std::cout << "Failed to load NVML library." << std::endl;
		throw std::invalid_argument("加载英伟达显卡驱动dll失败");
	}

	// 加载函数
	nvmlInit_f = (nvmlInit_t)GetProcAddress(nvmlLibrary, "nvmlInit_v2");
	nvmlShutdown_f = (nvmlShutdown_t)GetProcAddress(nvmlLibrary, "nvmlShutdown");
	nvmlDeviceGetCount_f = (nvmlDeviceGetCount_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetCount_v2");
	nvmlDeviceGetHandleByIndex_f = (nvmlDeviceGetHandleByIndex_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetHandleByIndex_v2");
	nvmlDeviceGetTemperature_f = (nvmlDeviceGetTemperature_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetTemperature");
	nvmlDeviceGetMemoryInfo_f = (nvmlDeviceGetMemoryInfo_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetMemoryInfo");
	nvmlDeviceGetUtilizationRates_f = (nvmlDeviceGetUtilizationRates_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetUtilizationRates");
	

	// 判断是否加载成功
	if (!nvmlInit_f || !nvmlShutdown_f || !nvmlDeviceGetCount_f || !nvmlDeviceGetHandleByIndex_f || !nvmlDeviceGetTemperature_f || !nvmlDeviceGetMemoryInfo_f || !nvmlDeviceGetUtilizationRates_f) {
		throw std::invalid_argument("加载英伟达显卡驱动dll失败");
	}

	result = nvmlInit_f();
	if (result != NVML_SUCCESS) {
		// std::cout << "Failed to initialize NVML: " << nvmlErrorString(result) << std::endl;
		throw std::invalid_argument("加载英伟达显卡驱动dll失败");
	}
	gpuVendor = gpu_nvidia;
}

void Gpu::amdInit() {
	throw std::invalid_argument("暂时无法检测AMD显卡");
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
			throw std::invalid_argument("无法获取英伟达显卡数量");
			FreeLibrary(nvmlLibrary);
			nvmlShutdown_f();
		}

		if (deviceCount > 0) {
			// 注意：英伟达驱动可能返回多张识别到的显卡。但本程序服务于一般台式家用电脑，只识别一张。变量i的值为返回的显卡标识
			int i = 0;
//
			nvmlDevice_t device;
//			char name[NVML_DEVICE_NAME_BUFFER_SIZE];
//
			result = nvmlDeviceGetHandleByIndex_f(i, &device);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get device handle: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("无法获取英伟达显卡操作句柄");
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
				throw std::invalid_argument("无法获取英伟达显卡温度");
			}

			nvmlMemory_t memory;
			result = nvmlDeviceGetMemoryInfo_f(device, &memory);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get memory info: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("无法获取英伟达显卡内存信息");
			}

			nvmlUtilization_t utilization;
			result = nvmlDeviceGetUtilizationRates_f(device, &utilization);
			if (result != NVML_SUCCESS) {
				// std::cout << "Failed to get utilization rates: " << nvmlErrorString(result) << std::endl;
				throw std::invalid_argument("无法获取英伟达显卡利用率");
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