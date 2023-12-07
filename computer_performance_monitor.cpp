// computer_performance_monitor.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "computer_performance_monitor.h"
#include "hardware.h"
#include <process.h>
#include "serialPorts.h"
#define MAX_LOADSTRING 100
// 定义自定义消息
#define WM_REFRESH_INFO_END (WM_USER + 1)

// 全局变量:
HWND hWnd;
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
static HFONT hFont30;
static HFONT hFontBold20;
static HFONT hFont20;
HWND hLabeHardwareStatusInfoGpuInfo3Value;
HWND hLabeHardwareStatusInfoGpuInfo2Value;
HWND hLabeHardwareStatusInfoGpuInfo1Value;
HWND hLabeHardwareStatusInfoMemoryInfo1Value;
HWND hLabeHardwareStatusInfoCpuInfo2Value;
HWND hLabeHardwareStatusInfoCpuInfo1Value;
HWND hLabeConnentInfo;
HWND listbox_hwnd;
Hardware* hardware;
SerialPorts* serialPorts;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

wchar_t  cpuInfo1Value[20];
wchar_t  cpuInfo2Value[20];
wchar_t  memoryInfo1Value[20];
wchar_t  gpuInfo1Value[20];
wchar_t  gpuInfo2Value[20];
wchar_t  gpuInfo3Value[20];
wchar_t  connentInfoValue[20];
wchar_t  logtxt[64];
unsigned __stdcall refreshInfoThreadFunc(void* data) {

	while (true)
	{
		Sleep(600);
		CpuInfo cpuInfo = hardware->getCpuInfo();
		// 处理器占用率
		swprintf_s(cpuInfo1Value, 10, L"%d%%", cpuInfo.load);

		// 处理器温度
		swprintf_s(cpuInfo2Value, 10, L"%d℃", cpuInfo.temperature);

		RamInfo ramInfo = hardware->getRamInfo();
		// 内存占用率
		swprintf_s(memoryInfo1Value, 20, L"%d%% （%dGB/%dGB）", ramInfo.load, ramInfo.used, ramInfo.total);

		// 获取GPU信息
		GpuInfo gpuInfo = hardware->getGpuInfo();
		gpuInfo = hardware->getGpuInfo();
		swprintf_s(gpuInfo1Value, 20, L"%d%%", gpuInfo.load);
		swprintf_s(gpuInfo2Value, 20, L"%d℃", gpuInfo.temperature);
		swprintf_s(gpuInfo3Value, 20, L"%d%% （%dGB/%dGB）", gpuInfo.ramInfo.load, gpuInfo.ramInfo.used, gpuInfo.ramInfo.total);
		swprintf_s(connentInfoValue, 20, serialPorts->isLink ? L"已连接" : L"未连接");
		
		try
		{
			serialPorts->writeData(0x5aa501, cpuInfo.temperature);
			serialPorts->writeData(0x5aa502, cpuInfo.load);
			serialPorts->writeData(0x5aa503, ramInfo.load);
			serialPorts->writeData(0x5aa504, ramInfo.used);
			serialPorts->writeData(0x5aa505, ramInfo.total);
			serialPorts->writeData(0x5aa506, gpuInfo.temperature);
			serialPorts->writeData(0x5aa507, gpuInfo.load);
			serialPorts->writeData(0x5aa508, gpuInfo.ramInfo.load);
			serialPorts->writeData(0x5aa509, gpuInfo.ramInfo.used);
			serialPorts->writeData(0x5aa50a, gpuInfo.ramInfo.total);
		}
		catch (const std::exception&e)
		{
			std::string str = "[异常] ";
			str += e.what();
			SendMessageA(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)str.c_str());
		}
		
		PostMessage(hWnd, WM_REFRESH_INFO_END, 0, 0);
	}

	return 0;
}

void refreshInfo() {

	uintptr_t threadHandle = _beginthreadex(NULL, 0, &refreshInfoThreadFunc, NULL, 0, NULL);
	if (threadHandle == -1) {
		// 处理创建子线程失败的情况
	}
	else {
		// 关闭线程句柄
		if (threadHandle != 0) {
			CloseHandle(reinterpret_cast<HANDLE>(threadHandle));
		}
	}

}

//设置当前程序开机启动
void AutoPowerOn()
{
	HKEY hKey;
	//std::string strRegPath = "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run";


	//1、找到系统的启动项  
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///打开启动项       
	{
		//2、得到本程序自身的全路径
		TCHAR strExeFullDir[MAX_PATH];
		GetModuleFileName(NULL, strExeFullDir, MAX_PATH);


		//3、判断注册表项是否已经存在
		TCHAR strDir[MAX_PATH] = {};
		DWORD nLength = MAX_PATH;
		long result = RegGetValue(hKey, nullptr, _T("computer_performance_monitor"), RRF_RT_REG_SZ, 0, strDir, &nLength);


		//4、已经存在
		if (result != ERROR_SUCCESS || _tcscmp(strExeFullDir, strDir) != 0)
		{
			//5、添加一个子Key,并设置值，"computer_performance_monitor"是应用程序名字（不加后缀.exe） 
			RegSetValueEx(hKey, _T("computer_performance_monitor"), 0, REG_SZ, (LPBYTE)strExeFullDir, (lstrlen(strExeFullDir) + 1) * sizeof(TCHAR));


			//6、关闭注册表
			RegCloseKey(hKey);
		}
	}
}


//取消当前程序开机启动
void CanclePowerOn()
{
	HKEY hKey;
	//std::string strRegPath = "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run";


	//1、找到系统的启动项  
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		//2、删除值
		RegDeleteValue(hKey, _T("computer_performance_monitor"));


		//3、关闭注册表
		RegCloseKey(hKey);
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, L"ComputerPerformanceMonitorUniqueMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS && hMutex != 0)
	{
		// 互斥体已经存在，表示程序已经在运行，用来限制程序只能同时启动一个实例
		CloseHandle(hMutex);
		return 0;
	}

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 开机自动运行
	AutoPowerOn();

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_COMPUTERPERFORMANCEMONITOR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COMPUTERPERFORMANCEMONITOR));

	swprintf_s(logtxt, L"[...] 初始化通信串口");
	SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)logtxt);
	std::string str = "[异常] ";
	try
	{
		serialPorts = new SerialPorts();
	}
	catch (const std::exception&e)
	{
		str += e.what();
		SendMessageA(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)str.c_str());
	}
	swprintf_s(logtxt, L"[成功] 初始化通信串口完成");
	SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)logtxt);

	swprintf_s(logtxt, L"[...] 初始化硬件性能读取模块");
	SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)logtxt);
	try
	{
		hardware = new Hardware;
		hardware->getCpuInfo();
		hardware->getRamInfo();
		hardware->getGpuInfo();
	}
	catch (const std::exception&e)
	{
		str += e.what();
		SendMessageA(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)str.c_str());
	}
	swprintf_s(logtxt, L"[提示] 读数为0表示没有找到对应设备");
	SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)logtxt);
	swprintf_s(logtxt, L"[成功] 初始化硬件性能读取模块完成");
	SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)logtxt);

	refreshInfo();

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOGO_32));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_LOGO_32));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, 400, 400, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	// 创建并设置任务栏图标
	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOGO_32));
	nid.uCallbackMessage = WM_USER;
	lstrcpy(nid.szTip, L"性能监视器");

	Shell_NotifyIcon(NIM_ADD, &nid);

	// 创建字体句柄
	hFont30 = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"微软雅黑");

	hFontBold20 = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"微软雅黑");

	hFont20 = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"微软雅黑");

	// 创建静态文本控件
	HWND hLabeTitle = CreateWindowEx(0, L"STATIC", L"性能监视器 v1.0", WS_VISIBLE | WS_CHILD,
		12, 12, 180, 26, hWnd, NULL, NULL, NULL);

	// 设置字体
	SendMessage(hLabeTitle, WM_SETFONT, (WPARAM)hFont30, TRUE);

	HWND hLabeConnent = CreateWindowEx(0, L"STATIC", L"设备连接状态：", WS_VISIBLE | WS_CHILD,
		12, 54, 130, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeConnent, WM_SETFONT, (WPARAM)hFontBold20, TRUE);

	hLabeConnentInfo = CreateWindowEx(0, L"STATIC", L"未连接", WS_VISIBLE | WS_CHILD,
		12 + 130, 54, 130, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeConnentInfo, WM_SETFONT, (WPARAM)hFont20, TRUE);

	HWND hLabeHardwareStatus = CreateWindowEx(0, L"STATIC", L"硬件状态：", WS_VISIBLE | WS_CHILD,
		12, 78, 130, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatus, WM_SETFONT, (WPARAM)hFontBold20, TRUE);

	HWND hLabeHardwareStatusInfoCpuTitle = CreateWindowEx(0, L"STATIC", L"处理器", WS_VISIBLE | WS_CHILD,
		20, 118, 46, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoCpuTitle, WM_SETFONT, (WPARAM)hFont20, TRUE);

	HWND hLabeHardwareStatusInfoCpuInfo1 = CreateWindowEx(0, L"STATIC", L"使用率", WS_VISIBLE | WS_CHILD,
		80, 106, 50, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoCpuInfo1, WM_SETFONT, (WPARAM)hFont20, TRUE);

	hLabeHardwareStatusInfoCpuInfo1Value = CreateWindowEx(0, L"STATIC", L"加载中", WS_VISIBLE | WS_CHILD,
		170, 106, 180, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoCpuInfo1Value, WM_SETFONT, (WPARAM)hFont20, TRUE);

	HWND hLabeHardwareStatusInfoCpuInfo2 = CreateWindowEx(0, L"STATIC", L"温度", WS_VISIBLE | WS_CHILD,
		80, 130, 50, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoCpuInfo2, WM_SETFONT, (WPARAM)hFont20, TRUE);

	hLabeHardwareStatusInfoCpuInfo2Value = CreateWindowEx(0, L"STATIC", L"加载中", WS_VISIBLE | WS_CHILD,
		170, 130, 180, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoCpuInfo2Value, WM_SETFONT, (WPARAM)hFont20, TRUE);

	HWND hLabeHardwareStatusInfoMemoryTitle = CreateWindowEx(0, L"STATIC", L"内存", WS_VISIBLE | WS_CHILD,
		26, 154, 30, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoMemoryTitle, WM_SETFONT, (WPARAM)hFont20, TRUE);

	HWND hLabeHardwareStatusInfoMemoryInfo1 = CreateWindowEx(0, L"STATIC", L"占用率", WS_VISIBLE | WS_CHILD,
		80, 154, 50, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoMemoryInfo1, WM_SETFONT, (WPARAM)hFont20, TRUE);

	hLabeHardwareStatusInfoMemoryInfo1Value = CreateWindowEx(0, L"STATIC", L"加载中", WS_VISIBLE | WS_CHILD,
		170, 154, 180, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoMemoryInfo1Value, WM_SETFONT, (WPARAM)hFont20, TRUE);

	HWND hLabeHardwareStatusInfoGpuTitle = CreateWindowEx(0, L"STATIC", L"显卡", WS_VISIBLE | WS_CHILD,
		26, 202, 30, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoGpuTitle, WM_SETFONT, (WPARAM)hFont20, TRUE);

	HWND hLabeHardwareStatusInfoGpuInfo1 = CreateWindowEx(0, L"STATIC", L"使用率", WS_VISIBLE | WS_CHILD,
		80, 178, 50, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoGpuInfo1, WM_SETFONT, (WPARAM)hFont20, TRUE);

	hLabeHardwareStatusInfoGpuInfo1Value = CreateWindowEx(0, L"STATIC", L"加载中", WS_VISIBLE | WS_CHILD,
		170, 178, 180, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoGpuInfo1Value, WM_SETFONT, (WPARAM)hFont20, TRUE);

	HWND hLabeHardwareStatusInfoGpuInfo2 = CreateWindowEx(0, L"STATIC", L"核心温度", WS_VISIBLE | WS_CHILD,
		80, 202, 70, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoGpuInfo2, WM_SETFONT, (WPARAM)hFont20, TRUE);

	hLabeHardwareStatusInfoGpuInfo2Value = CreateWindowEx(0, L"STATIC", L"加载中", WS_VISIBLE | WS_CHILD,
		170, 202, 180, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoGpuInfo2Value, WM_SETFONT, (WPARAM)hFont20, TRUE);

	HWND hLabeHardwareStatusInfoGpuInfo3 = CreateWindowEx(0, L"STATIC", L"显存占用率", WS_VISIBLE | WS_CHILD,
		80, 226, 70, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoGpuInfo3, WM_SETFONT, (WPARAM)hFont20, TRUE);

	hLabeHardwareStatusInfoGpuInfo3Value = CreateWindowEx(0, L"STATIC", L"加载中", WS_VISIBLE | WS_CHILD,
		170, 226, 180, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeHardwareStatusInfoGpuInfo3Value, WM_SETFONT, (WPARAM)hFont20, TRUE);

	HWND hLabeLog = CreateWindowEx(0, L"STATIC", L"运行日志：", WS_VISIBLE | WS_CHILD,
		12, 258, 130, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hLabeLog, WM_SETFONT, (WPARAM)hFontBold20, TRUE);

	// 创建多项列表控件
	listbox_hwnd = CreateWindow(
		L"LISTBOX",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
		10,
		282,
		370,
		70,
		hWnd,
		NULL,
		hInstance,
		NULL
	);

	
	swprintf_s(logtxt, L"[成功] 程序启动");
	SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)logtxt);

	// ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case 1:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_CLOSE:
		// 点击任务栏图标的关闭按钮时关闭程序
		// PostQuitMessage(0);
		ShowWindow(hWnd, SW_HIDE);
		return 0;
	case WM_USER:
		// 处理自定义消息
		// TODO: 在这里执行你的自定义操作
		if (lParam == WM_LBUTTONUP)
		{
			// 处理任务栏图标的左键点击事件
			if (!IsWindowVisible(hWnd))
			{
				// 窗口不可见时，重新显示窗口
				ShowWindow(hWnd, SW_SHOW);
			}
		}
		if (lParam == WM_RBUTTONUP)
		{
			// 处理任务栏图标的右键点击事件
			POINT cursorPos;
			GetCursorPos(&cursorPos);

			HMENU hPopupMenu = CreatePopupMenu();
			// 添加子菜单项
			AppendMenu(hPopupMenu, MF_STRING, 1, L"退出程序");
			// ...

			// 显示悬浮子菜单
			TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursorPos.x, cursorPos.y, 0, hWnd, NULL);
		}
		return 0;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetBkMode(hdcStatic, TRANSPARENT);
		SetBkColor(hdcStatic, RGB(255, 255, 255));  // 设置白色背景
		return (INT_PTR)GetStockObject(WHITE_BRUSH);
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc1 = BeginPaint(hWnd, &ps);
		// 创建画笔
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		HPEN hOldPen = (HPEN)SelectObject(hdc1, hPen);
		// 设置起始点并绘制直线
		MoveToEx(hdc1, 12, 104, NULL);
		LineTo(hdc1, 370, 104);
		MoveToEx(hdc1, 70, 128, NULL);
		LineTo(hdc1, 370, 128);
		MoveToEx(hdc1, 12, 152, NULL);
		LineTo(hdc1, 370, 152);
		MoveToEx(hdc1, 12, 176, NULL);
		LineTo(hdc1, 370, 176);
		MoveToEx(hdc1, 70, 200, NULL);
		LineTo(hdc1, 370, 200);
		MoveToEx(hdc1, 70, 224, NULL);
		LineTo(hdc1, 370, 224);
		MoveToEx(hdc1, 12, 248, NULL);
		LineTo(hdc1, 370, 248);
		MoveToEx(hdc1, 12, 104, NULL);
		LineTo(hdc1, 12, 248);
		MoveToEx(hdc1, 70, 104, NULL);
		LineTo(hdc1, 70, 248);
		MoveToEx(hdc1, 160, 104, NULL);
		LineTo(hdc1, 160, 248);
		MoveToEx(hdc1, 370, 104, NULL);
		LineTo(hdc1, 370, 248);

		// 清理
		SelectObject(hdc1, hOldPen);
		DeleteObject(hPen);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SYSCOMMAND:
		if (wParam == SC_MINIMIZE)
		{
			// 拦截最小化命令
			return 0;
		}
		else if (wParam == SC_MAXIMIZE)
		{
			// 拦截全屏命令
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_REFRESH_INFO_END: {
		SetWindowText(hLabeHardwareStatusInfoCpuInfo1Value, cpuInfo1Value);
		SetWindowText(hLabeHardwareStatusInfoCpuInfo2Value, cpuInfo2Value);
		SetWindowText(hLabeHardwareStatusInfoMemoryInfo1Value, memoryInfo1Value);
		SetWindowText(hLabeHardwareStatusInfoGpuInfo1Value, gpuInfo1Value);
		SetWindowText(hLabeHardwareStatusInfoGpuInfo2Value, gpuInfo2Value);
		SetWindowText(hLabeHardwareStatusInfoGpuInfo3Value, gpuInfo3Value);
		SetWindowText(hLabeConnentInfo, connentInfoValue);
		return 0;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
