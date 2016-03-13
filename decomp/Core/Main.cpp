int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	InitCommonControls();
	hInstApp = hInstance;

	zSTRING cmdLine(lpCmdLine);
	zERROR::Init(zerr, cmdLine);

	cmdLine.Upper();
	if ( cmdLine.Search(0, "ZNOEXHND", 1u) == -1 ) {
		zExHandler = 1;
		zCExceptionHandler::SetIsActive(1);
	} else {
		zExHandler = 0;
	}

	return Main(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

int Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return HandledWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

void netInit()
{
}

HANDLE sysInstLock;
char sysLogName[260];

void winDoInstanceCheck()
{
	char mapLockName[260];

	sprintf(mapLockName, "Lock_%s", REG_PROGRAM);
	sysInstLock = CreateFileMappingA((HANDLE)-1, 0, 2u, 0, 0x20u, mapLockName);

	if ( sysInstLock ) {
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
			ExitProcess(1u);
	}
}

int HandledWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	hInstApp = hInstance;

	winDoInstanceCheck();

	strcpy(sysLogName, HFILE_LOG);

	winMainThreadID = GetCurrentThreadId();

	DuplicateHandle(GetCurrentProcess(),
	                GetCurrentThread(),
	                GetCurrentProcess(),
	                &winMainThread, 0, 0, 2u);

	LARGE_INTEGER PerformanceCount;
	DWORD Time;
	DWORD Time70;

	QueryPerformanceFrequency(&Frequency);
	if ( Frequency.LowPart < 1000 || Frequency.HighPart ) {
		PreciseTimer = 0;
		Time = timeGetTime();
	} else {
		pTimeDiv = Frequency.LowPart / 1000;
		pTimeDiv70 = Frequency.LowPart / 70;
		PreciseTimer = 1;
		QueryPerformanceCounter(&PerformanceCount);
		PerformanceCount.LowPart = PerformanceCount.QuadPart /pTimeDiv;
		Time = PerformanceCount.LowPart;
	}
	BeginTime = Time;

	if ( PreciseTimer ) {
		QueryPerformanceCounter(&PerformanceCount);
		PerformanceCount.LowPart = PerformanceCount.QuadPart / pTimeDiv70;
		Time70 = PerformanceCount.LowPart;
	} else {
		Time70 = timeGetTime() / 0xE;
	}
	BeginTime70 = Time70;

	sysParseCommandLine(lpCmdLine);
	hHandle = CreateMutexA(0, 0, 0);
	GetModuleFileNameA(0, Filename, 0x104u);
	InitCommonControls();

	memset(&mouseInf, 0, 0x18Cu);

	checkWinVersion();

	dcScreen = CreateCompatibleDC(0);

	winExtraX = 2 * GetSystemMetrics(32) + 5;
	winExtraY = 2 * GetSystemMetrics(33) + 5 + GetSystemMetrics(4) ;

	if ( !vidHideMenu )
		winExtraY += GetSystemMetrics(15);

	WNDCLASSA WndClass;
	memset(&WndClass, 0, sizeof(WndClass));

	hIconApp = LoadIconA(hInstApp, (LPCSTR)0xA3);

	WndClass.style = 512;
	WndClass.lpfnWndProc = (WNDPROC)AppWndProc;
	WndClass.hInstance = hInstApp;
	WndClass.hIcon = hIconApp;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(4);
	WndClass.lpszClassName = "DDWndClass";

	if ( !RegisterClassA(&WndClass) )
		sysHardExit(Format);

	memset(&WndClass, 0, sizeof(WndClass));

	WndClass.lpfnWndProc = netWindowProc;
	WndClass.hInstance = hInstApp;
	WndClass.lpszClassName = "DDWinSock";

	if ( !RegisterClassA(&WndClass) )
		sysHardExit(Format);

	byte_8D4110 = 0;

	Y = 0;
	iwPos = 0;

	if ( SystemParametersInfoA(0x30u, 0, &pvParam, 0) && pvParam >= 0 && v16 >= 0 )
	{
		iwPos = pvParam;
		Y = v16;
	}

	dword_8D4100 = 0x90000000;
	dword_8D40FC = 0x90000000;
	dword_8D4108 = 0x90000000;
	dword_8D4104 = 0x90000000;

	nWidth  = winExtraX + 800;
	nHeight = winExtraY + 600;

	dword_8D410C = 0;
	dword_8D40F8 = 0;

	hWndApp = CreateWindowExA(
	                0x300u, "DDWndClass", APP_NAME, 0xC80000u,
	                iwPos, Y, winExtraX + 800, winExtraY + 600,
	                0, 0, hInstApp, 0);

	if ( hWndApp ) {
		netInit();
		TRACE("---- Starting execution: invoke MainProg()");
		MainProg();
		libExit();
	}
}

int MainProg()
{
	CGameManager game;

	sysEvent();
	
	game.Init((HWND *)&hWndApp);
	game.Run();
	game.Done();

	return 0;
}
