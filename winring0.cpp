#include "winring0.h"

WinRing0::WinRing0() {
	if (InitializeOls() == false) {
		DWORD str = GetDllStatus();//获取失败原因代码
		switch (GetDllStatus())
		{
		case OLS_DLL_NO_ERROR:
			break;
		case OLS_DLL_UNSUPPORTED_PLATFORM:
			throw std::invalid_argument("底层驱动DLL初始化失败，平台不支持");
			break;
		case OLS_DLL_DRIVER_NOT_LOADED:
			throw std::invalid_argument("底层驱动DLL初始化失败，驱动没有加载");
			break;
		case OLS_DLL_DRIVER_NOT_FOUND:
			throw std::invalid_argument("底层驱动DLL初始化失败，驱动找不到");
			break;
		case OLS_DLL_DRIVER_UNLOADED:
			throw std::invalid_argument("底层驱动DLL初始化失败，驱动已卸载");
			break;
		case OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK:
			throw std::invalid_argument("底层驱动DLL初始化失败，网络上没有驱动程序");
			break;
		case OLS_DLL_UNKNOWN_ERROR:
		default:
			throw std::invalid_argument("底层驱动DLL初始化失败，驱动找不到，未知的错误");
			break;
		}
		throw std::invalid_argument("底层驱动DLL初始化失败，初始化失败");
	}
}

WinRing0::~WinRing0() {
	DeinitializeOls();
}