#include "wmi.h"

Wmi::Wmi()
{
    hres = CoInitializeEx(0, COINIT_MULTITHREADED); // 初始化 COM 组件
    if (FAILED(hres))
    {
        // std::cout << "Failed to initialize COM library. Error code: " << std::hex << hres << std::endl;
        throw std::invalid_argument("Failed to initialize COM library.");
    }

    hres = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL
    );

    if (FAILED(hres))
    {
        // std::cout << "Failed to initialize security. Error code: " << std::hex << hres << std::endl;
        CoUninitialize();
        throw std::invalid_argument("Failed to initialize security.");
    }

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID*)&pLoc
    );
    if (FAILED(hres))
    {
        // std::cout << "Failed to create IWbemLocator object. Error code: " << std::hex << hres << std::endl;
        CoUninitialize();
        throw std::invalid_argument("Failed to create IWbemLocator object. Error code.");
    }

    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // WMI 命名空间
        NULL, // 用户名
        NULL, // 密码
        NULL, // 本地机器上下文
        0, // 安全标志
        NULL, // 权限
        NULL, // 操作者上下文
        &pSvc // IWbemServices 实例
    );
    if (FAILED(hres))
    {
        // std::cout << "Could not connect to WMI. Error code: " << std::hex << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        throw std::invalid_argument("Could not connect to WMI.");
    }

    // 设置安全级别
    hres = CoSetProxyBlanket(
        pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE
    );

    if (FAILED(hres))
    {
        // std::cout << "Could not set security levels. Error code: " << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        throw std::invalid_argument("Could not set security levels.");
    }
}

Wmi::~Wmi()
{
    // 释放资源
    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
}

int Wmi::getCpuLoadForWmi() {

    // 执行 WMI 查询
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT LoadPercentage FROM Win32_Processor"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator
    );
    if (FAILED(hres))
    {
        // std::cout << "Could not execute WQL query. Error code: " << std::hex << hres << std::endl;
        //pSvc->Release();
        //pLoc->Release();
        //CoUninitialize();
        throw std::invalid_argument("Could not execute WQL query.");
        return 0;
    }

    int cpuLoad = 0;

    // 获取查询结果
    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;
    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (0 == uReturn)
            break;

        VARIANT vtProp;
        hr = pclsObj->Get(L"LoadPercentage", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr))
        {
            cpuLoad = vtProp.uintVal;
            // std::cout << "CPU 负载百分比：" << vtProp.uintVal << std::endl;
            VariantClear(&vtProp);
        }
        else {
            throw std::invalid_argument("Could not execute WQL query.");
        }

        pclsObj->Release();
    }

    return cpuLoad;
}