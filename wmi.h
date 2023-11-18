#pragma once
#include <Wbemidl.h>
#include <windows.h>
#include <stdexcept>
#include <comdef.h> // 用于处理 COM 错误
#pragma comment(lib, "wbemuuid.lib") // 要连接 WMI，需要链接 wbemuuid 库
class  Wmi
{
public:
	 Wmi();
	~ Wmi();
	int getCpuLoadForWmi();

private:
	HRESULT hres;
	IWbemServices* pSvc = NULL;
	IWbemLocator* pLoc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
};