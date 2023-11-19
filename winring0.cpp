#include "winring0.h"

WinRing0::WinRing0() {
	if (InitializeOls() == false) {
		DWORD str = GetDllStatus();//��ȡʧ��ԭ�����
		switch (GetDllStatus())
		{
		case OLS_DLL_NO_ERROR:
			break;
		case OLS_DLL_UNSUPPORTED_PLATFORM:
			throw std::invalid_argument("�ײ�����DLL��ʼ��ʧ�ܣ�ƽ̨��֧��");
			break;
		case OLS_DLL_DRIVER_NOT_LOADED:
			throw std::invalid_argument("�ײ�����DLL��ʼ��ʧ�ܣ�����û�м���");
			break;
		case OLS_DLL_DRIVER_NOT_FOUND:
			throw std::invalid_argument("�ײ�����DLL��ʼ��ʧ�ܣ������Ҳ���");
			break;
		case OLS_DLL_DRIVER_UNLOADED:
			throw std::invalid_argument("�ײ�����DLL��ʼ��ʧ�ܣ�������ж��");
			break;
		case OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK:
			throw std::invalid_argument("�ײ�����DLL��ʼ��ʧ�ܣ�������û����������");
			break;
		case OLS_DLL_UNKNOWN_ERROR:
		default:
			throw std::invalid_argument("�ײ�����DLL��ʼ��ʧ�ܣ������Ҳ�����δ֪�Ĵ���");
			break;
		}
		throw std::invalid_argument("�ײ�����DLL��ʼ��ʧ�ܣ���ʼ��ʧ��");
	}
}

WinRing0::~WinRing0() {
	DeinitializeOls();
}