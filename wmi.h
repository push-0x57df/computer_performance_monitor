#pragma once
#include <Wbemidl.h>
#include <windows.h>
#include <stdexcept>
#include <comdef.h> // ���ڴ��� COM ����
#pragma comment(lib, "wbemuuid.lib") // Ҫ���� WMI����Ҫ���� wbemuuid ��
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