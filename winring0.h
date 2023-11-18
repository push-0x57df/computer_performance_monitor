#pragma once
#include "windows.h"
#include "OlsApi.h"
#include <stdexcept>
#ifdef _M_X64
#pragma comment(lib, "WinRing0x64.lib")
#else if
#pragma comment(lib, "WinRing0.lib")
#endif

// winRing0驱动初始化需要的参数
#define OLS_DLL_NO_ERROR						0
#define OLS_DLL_UNSUPPORTED_PLATFORM			1
#define OLS_DLL_DRIVER_NOT_LOADED				2
#define OLS_DLL_DRIVER_NOT_FOUND				3
#define OLS_DLL_DRIVER_UNLOADED					4
#define OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK	5
#define OLS_DLL_UNKNOWN_ERROR					9

class WinRing0 {
private:

public:
	WinRing0();
	~WinRing0();
};