#define WIN32_LEAN_AND_MEAN
#include <Gothic/System/System.h>
#include <Gothic/System/Win32.h>
#include <Gothic/System/Video.h>
#include <windows.h>
#include <commctrl.h>

auto Frequency = Value<LARGE_INTEGER>(0x8D3FE0);

void InitWin32Stuff(char const* cmdLine)
{
	InitCommonControls();

	winMainThreadID = GetCurrentThreadId();
	auto a = GetCurrentProcess();
	auto b = GetCurrentThread();
	auto c = GetCurrentProcess();
	DuplicateHandle(a, b, c, &winMainThread, 0, 0, 2u);

	winDoInstanceCheck();

	LARGE_INTEGER PerformanceCount;
	DWORD Time;
	DWORD Time70;

	QueryPerformanceFrequency(&Frequency);
	pTimeDiv = Frequency.LowPart / 1000;
	pTimeDiv70 = Frequency.LowPart / 70;
	PreciseTimer = 1;
	QueryPerformanceCounter(&PerformanceCount);
	PerformanceCount.LowPart = PerformanceCount.QuadPart /pTimeDiv;
	Time = PerformanceCount.LowPart;
	BeginTime = Time;

	QueryPerformanceCounter(&PerformanceCount);
	PerformanceCount.LowPart = PerformanceCount.QuadPart / pTimeDiv70;
	Time70 = PerformanceCount.LowPart;
	BeginTime70 = Time70;

	sysParseCommandLine(cmdLine);
	hHandle = CreateMutexA(0, 0, 0);
	GetModuleFileNameA(0, winModuleName, 0x104u);

	memset((char*)&mouseInf, 0, 0x18Cu);

	checkWinVersion();

	dcScreen = CreateCompatibleDC(0);

	winExtraX = 2 * GetSystemMetrics(32) + 5;
	winExtraY = 2 * GetSystemMetrics(33) + 5 + GetSystemMetrics(4) ;

	if ( !vidHideMenu )
		winExtraY += GetSystemMetrics(15);

	WNDCLASSA WndClass;
	memset(&WndClass, 0, sizeof(WndClass));

	hIconApp = LoadIconA((HINSTANCE)hInstApp, (LPCSTR)0xA3);

	WndClass.style = 512;
	WndClass.lpfnWndProc = (WNDPROC)AppWndProc;
	WndClass.hInstance = (HINSTANCE)hInstApp;
	WndClass.hIcon = (HICON)hIconApp;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(4);
	WndClass.lpszClassName = "DDWndClass";

	if ( !RegisterClassA(&WndClass) )
		sysHardExit("Could not register window class");
/* network is unused
	memset(&WndClass, 0, sizeof(WndClass));

	WndClass.lpfnWndProc = netWindowProc;
	WndClass.hInstance = hInstApp;
	WndClass.lpszClassName = "DDWinSock";

	if ( !RegisterClassA(&WndClass) )
		sysHardExit(Format);
	*/

	RECT rect;
	winPosX = 0;
	winPosY = 0;

	auto res = SystemParametersInfoA(SPI_GETWORKAREA, 0, &rect, 0);
	if ( res && rect.left >= 0 && rect.top >= 0 ) {
		winPosX = rect.left;
		winPosY = rect.top;
	}

	nWidth  = winExtraX + 800;
	nHeight = winExtraY + 600;

	hWndApp = CreateWindowExA(
	                WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE,
			"DDWndClass",
			APP_NAME,
			WS_BORDER|WS_SYSMENU|WS_DLGFRAME,
	                winPosX, winPosY,
			nWidth, nHeight,
	                0, 0, (HINSTANCE)hInstApp, 0);
}
