#include <Gothic/System/System.h>
#include <Gothic/System/Win32.h>
#include <Gothic/System/Video.h>
#include <Logging/Log.h>

#include <aw/utility/unicode/convert.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

auto& Frequency = Value<LARGE_INTEGER>(0x8D3FE0);
auto& hIconApp = Value<HICON>(0x8D4228);

WNDPROC AppWndProc    = (WNDPROC)0x503770;
WNDPROC netWindowProc = (WNDPROC)0x4562F0;
//auto& sysStopProcessingMessages = Value<int>(0x8D4234);
//auto HandleFocusLoose = Cdecl<void()>(0x503630);

int& winExtraX = Value<int>(0x8D3980);
int& winExtraY = Value<int>(0x8D3988);
int& winPosX = Value<int>(0x8D40E8);
int& winPosY = Value<int>(0x8D40EC);
int& nWidth  = Value<int>(0x8D40F0);
int& nHeight = Value<int>(0x8D40F4);

using WINAPI_STRING = std::basic_string<WCHAR>;

WINAPI_STRING widen(std::string const& str)
{
	using namespace aw::unicode;
	return convert<WINAPI_STRING>(str, utf8{}, utf16{});
}

std::string narrow(WINAPI_STRING const& str)
{
	using namespace aw::unicode;
	return convert<std::string>(str, utf16{}, utf8{});
}

void InitWin32Stuff(char const* cmdLine)
{
	using namespace g2;
	Log("Win32", "Initializing");
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

	memset((char*)0x8D3BA0, 0, 0x18Cu);

	checkWinVersion();

	dcScreen = CreateCompatibleDC(0);

	winExtraX = 2 * GetSystemMetrics(32) + 5;
	winExtraY = 2 * GetSystemMetrics(33) + 5 + GetSystemMetrics(4) ;

	if ( !vidHideMenu )
		winExtraY += GetSystemMetrics(15);

	WNDCLASSW WndClass;
	memset(&WndClass, 0, sizeof(WndClass));

	hIconApp = LoadIconW((HINSTANCE)hInstApp, (LPCWSTR)0xA3);

	WndClass.style = 512;
	WndClass.lpfnWndProc   = DefWindowProcW; //AppWndProc;
	WndClass.hInstance     = (HINSTANCE)hInstApp;
	WndClass.hIcon         = (HICON)hIconApp;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(4);
	WndClass.lpszClassName = L"DDWndClass";

	if ( !RegisterClassW(&WndClass) )
		sysHardExit("Could not register window class");

	RECT rect;
	winPosX = 0;
	winPosY = 0;

	auto res = SystemParametersInfoW(SPI_GETWORKAREA, 0, &rect, 0);
	if ( res && rect.left >= 0 && rect.top >= 0 ) {
		winPosX = rect.left;
		winPosY = rect.top;
	}

	nWidth  = winExtraX + 800;
	nHeight = winExtraY + 600;

	Log("Win32", "Creating window");

	hWndApp = CreateWindowW(
			L"DDWndClass",
			widen(APP_NAME).data(),
			WS_BORDER|WS_SYSMENU|WS_DLGFRAME,
	                winPosX, winPosY,
			nWidth, nHeight,
	                0, 0, (HINSTANCE)hInstApp, 0);

	if (hInstApp)
		Log("Win32", "Window created: ", uintptr_t(hWndApp));
	else
		Error("Win32", "Could not create window: ", GetLastError());
}
