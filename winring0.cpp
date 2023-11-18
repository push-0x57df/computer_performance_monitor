#include "winring0.h"

WinRing0::WinRing0() {
	if (InitializeOls() == false) {
		DWORD str = GetDllStatus();//获取失败原因代码
		switch (GetDllStatus())
		{
		case OLS_DLL_NO_ERROR:
			break;
		case OLS_DLL_UNSUPPORTED_PLATFORM:
			throw std::invalid_argument("DLL Status Error!! UNSUPPORTED_PLATFORM");
			break;
		case OLS_DLL_DRIVER_NOT_LOADED:
			throw std::invalid_argument("DLL Status Error!! DRIVER_NOT_LOADED");
			break;
		case OLS_DLL_DRIVER_NOT_FOUND:
			throw std::invalid_argument("DLL Status Error!! DRIVER_NOT_FOUND");
			break;
		case OLS_DLL_DRIVER_UNLOADED:
			throw std::invalid_argument("DLL Status Error!! DRIVER_UNLOADED");
			break;
		case OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK:
			throw std::invalid_argument("DLL Status Error!! DRIVER_NOT_LOADED_ON_NETWORK");
			break;
		case OLS_DLL_UNKNOWN_ERROR:
		default:
			throw std::invalid_argument("DLL Status Error!! UNKNOWN_ERROR");
			break;
		}
		throw std::invalid_argument("Error InitializeOls()!!");
	}
}

WinRing0::~WinRing0() {
	DeinitializeOls();
}