unsigned sysGetTime()
{
	if ( PreciseTimer ) {
		LARGE_INTEGER PerformanceCount;
		QueryPerformanceCounter(&PerformanceCount);
		return = PerformanceCount.QuadPart / pTimeDiv - BeginTime;
	}

	return = timeGetTime() - BeginTime;
}

void libExit()
{
	if ( !inSysExit ) {
		inSysExit = 1;

		if ( GetCurrentThreadId() != winMainThreadID )
			SuspendThread(winMainThread);

		sysStopProcessingMessages = 1;

		TRACE("");
		TRACE("---- Exiting program");

		netExit();

		UnhookWindowsHookEx(hhk);

		while ( DeleteMenu(hMenu, 0, 0x400u) );

		TRACE("WIN: Closing window");

		if ( hWndApp )
			DestroyWindow(hWndApp);
		if ( hMenu )
			DestroyMenu(hMenu);
		if ( dcScreen )
			DeleteDC(dcScreen);
		if ( hKey )
			RegCloseKey(hKey);
		if ( sysInstLock )
			CloseHandle(sysInstLock);

		if ( sysCommandLine ) {
			LocalFree(sysCommandLine);
			sysCommandLine = 0;
		}

		sysKillWindowsKeys(0);

		if ( zExHandler )
			zCExceptionHandler::WalkReleaseCallbacks();

		TRACE("WIN: All done! Have a nice day");
	}
}

void [[noreturn]] sysExit()
{
	libExit();
	if ( winMainThread )
		CloseHandle(winMainThread);
	if ( hHandle )
		CloseHandle(hHandle);
	ExitProcess(0);
}

void [[noreturn]] sysHardExit(char* format, ...)
{
	char buf[512];
	va_list va;
	va_start(va, format);

	if ( !sysExiting ) {
		sysExiting = 1;

		vsprintf(buf, format, va);
		TRACE("WIN: Application notified error: %s", buf);

		if ( byte_89C86C == 5 )
			vidCloseDisplay(0);

		ShowWindow(hWndApp, 0);

		auto time = sysGetTime();

		vidCloseDisplay(0);
		ShowWindow(hWndApp, 0);

		DialogBoxParamA(hInstApp, 0x7B, 0, CrashDlgProc, &Dest);

		sysTimeGap += sysGetTime() - time;

		sysExit();
	}
}

bool didDebuggerCheck;
int (*gotDebugger)();
int sysGotDebugger()
{
	if (!didDebuggerCheck) {
		auto lib = LoadLibraryA("kernel32.dll");
		gotDebugger = GetProcAddress(lib, "IsDebuggerPresent");
		didDebuggerCheck = true;
	}

	return gotDebugger();
}

int sysHandleExceptions()
{
	return zExHandler;
}

void* sysAlloc(size_t size)
{
	void *result = LocalAlloc(0x40u, size);
	if ( !result ) {
		TRACE("WIN: Could not allocate %u KB memory block, negotiating with user");
		auto time = sysGetTime();
		bool minimize = byte_89C86C == 5;

		if ( minimize )
			vidMinimize(0);

		result = DialogBoxParamA(hInstApp, 0x8F, hWndApp, OutOfMemProc, size);

		if ( minimize )
			vidRestore(time, dbox, minimize);

		sysTimeGap += sysGetTime() - time;
		if ( !result  ) {
			TRACE("WIN: Amnesia Error: Out of memory, user canceled memory request");
			sysExit();
		}

		TRACE("WIN: Memory block succesfully allocated");
	}

	return result;
}

void* sysReAlloc(void* mem, size_t size)
{
	void* result = nullptr;
	if ( mem )
		result = LocalReAlloc(mem, size, 0x42u);
	else
		result = LocalAlloc(0x40, size);

	if (mem) {
		sysHardExit("Could not resize memory block %08X to %u Kb (%u zBYTEs)\nError code %d", hMem, (size + 512) >> 10, size, GetLastError());
	}

	return result;
}

void sysFree(void* mem)
{
	LocalFree(mem);
}

void sysSetWindowMode(int FullScreen, int Show)
{
	tagRECT Rect;
	if ( FullScreen ) {
		if ( winMode == 1 ) {
			GetWindowRect(hWndApp, &Rect);

			iwPos = Rect.left;
			Y = Rect.top;
			nWidth = Rect.right - Rect.left;
			nHeight = Rect.bottom - Rect.top;
			byte_8D4110 = 1;
		}

		SetMenu(hWndApp, 0);
		SetWindowLongA(hWndApp, -16, 2147483648);
		SetWindowLongA(hWndApp, -20, 8);
		winMode = 2;

		if ( Show ) {
			SetWindowPos(hWndApp, HWND_MESSAGE|0x2, 0, 0, GetSystemMetrics(0), GetSystemMetrics(1), 0x20u);
			if ( !IsZoomed(hWndApp) )
				ShowWindow(hWndApp, 5);
		}
	} else {
		if ( winMode == 2 && byte_8D4110 ) {
			ShowWindow(hWndApp, 0);
			MoveWindow(hWndApp, iwPos, Y, nWidth, nHeight, 1);
			winResizeMainWindow();
			ccRenderCaption(hWndApp, &ccMain);
		}

		SetWindowLongA(hWndApp, -16, 13107200);
		SetWindowLongA(hWndApp, -20, 768);
		winCreateMenu();
		SetWindowPos(hWndApp, 0, 0, 0, 0, 0, 3u);
		winMode = 1;
		ShowWindow(hWndApp, 5);
		SetForegroundWindowEx(hWndApp);
		UpdateWindow(hWndApp);
		InvalidateRect(GetDesktopWindow(), 0, 1);
	}
}

char sysLogName[260];

void sysParseCommandLine(char *cmd)
{
	char string[2048];

	auto tmp = sysAlloc(strlen(cmd) + 2);

	sysCommandLine = strcpy(mem, cmd);
	strcpy(string, cmd);
	_strlwr(string);

	if ( strstr(string, "-log=") ) {
		unsigned i = 5;
		unsigned begin = strstr(string, "-log=") - string;
		char c;
		v12 = sysCommandLine + v11;
		do {
			c = sysCommandLine[begin + i];
			sysLogName[i++ - 5] = c;
		} while ( c && c != 32 );

		memmove(&string[begin], &string[begin + i], strlen(string) - begin);
		memmove(&sysCommandLine[begin], &sysCommandLine[begin + i], strlen(sysCommandLine) - begin);
	}

	if ( CmdLineGotOption(string, "-safe", 1) )
		usrSafeMode = 1;
	if ( CmdLineGotOption(string, "-windowed", 1) )
		sysWindowMode = 1;
	if ( CmdLineGotOption(string, "-fullscreen", 1) )
		sysWindowMode = 2;
	if ( CmdLineGotOption(string, "-safelog", 1) )
		sysSafeLog = 1;
	if ( CmdLineGotOption(string, "-nonetwork", 1) )
		netEnable = 0;
	if ( CmdLineGotOption(string, "-noddrawwin", 1) )
		useDDrawWin = 0;
	if ( CmdLineGotOption(string, "-noddraw", 1) )
		useDDraw = 0;
	if ( CmdLineGotOption(string, "-noddrawmemcheck", 1) )
		vidCheckMemory = 0;
	if ( CmdLineGotOption(string, "-noreg", 1) )
		winUseRegistry = 0;

	if ( usrSafeMode ) {
		TRACE("WIN: Safe mode startup");
		sysWindowMode = 4;
		netEnable = 0;
		useDDraw = 0;
		useDDrawWin = 0;
	}
}

void sysKillWindowsKeys(bool a1);

void sysIdle(unsigned time_ms)
{
	Sleep(time_ms);
}

void sysProcessIdle()
{
	netAcceptPendingConnections();

	if ( sysCallBack ) {
		while ( 1 ) {
			auto time = sysGetTime();
			if ( time <= sysCallBackLast + sysCallBackDelay )
				break;

			sysCallBack(sysCallBackLast);

			if ( sysCallBackSingle ) {
				sysCallBackLast += sysCallBackDelay * ((time - sysCallBackLast) / sysCallBackDelay);
				break;
			}

			sysCallBackLast += sysCallBackDelay;
		}
	}
}

void sysSetFocus()
{
	SetWindowPos(hWndApp, 0, 0, 0, 0, 0, 0x40Bu);
	SetForegroundWindowEx(hWndApp);
	SetFocus(hWndApp);
	ShowWindow(hWndApp, 9);
	ShowWindow(hWndApp, 1);
}

void HandleFocusLoose()
{
	if ( zrenderer && zrenderer->Vid_GetScreenMode() == zRND_SCREEN_MODE_FULLSCREEN ) {
		zWARNING("C: lost focus in fullscreen, setting to windowed mode"); // 1623m _carsten\\zWin32.cpp

		zrenderer->SetSurfaceLost(1);

		if ( ogame )
			ogame->Pause(0);

		zrenderer->Vid_SetScreenMode(&zrenderer->vtab, zRND_SCREEN_MODE_WINDOWED);

		SetWindowPos(hWndApp, 0, 0, 0, 0, 0, 0x40B);
		focusLost = 1;
	}
}

void sysEvent()
{
	if ( !inSysEvent ) {
		inSysEvent = 1;

		sysProcessIdle();

		while ( PeekMessageA(&Msg, 0, 0, 0, 1u) ) {
			TranslateMessage(&Msg);
			DispatchMessageA(&Msg);
		}

		if ( sysEventScreenBlanked ) {
			vidWaitScreenUnblank();
		} else if ( sysEventToggleFullScreen || vidLostFocus() ) {
			vidToggleFullScreenMode(0);
		}

		sysEventToggleFullScreen = 0;
		sysEventScreenBlanked = 0;
		inSysEvent = 0;

		if (GetForegroundWindow() == hWndApp || zrenderer && zrenderer->Vid_GetScreenMode() ) {
			if ( focusLost ) {
				bool enableTaskSwitch = false;
				if (zoptions) {
					enableTaskSwitch = zoptions->ReadBool("RENDERER_D3D", "zEnableTaskSwitch", 1) || zoptions->Parm("DEVMODE");
				}

				if (enableTaskSwitch || GetActiveWindow() == hWndApp) {
					focusLost = 0;
					if ( zrenderer )
						zrenderer->Vid_SetScreenMode();

					sysSetFocus();

					if ( ogame )
						ogame->Unpause();
				}
			}
		} else {
			HandleFocusLoose();
		}
	}
}


HANDLE sysInstLock;

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

int Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (zExHandler)
		return HandledWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);

	return HandledWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

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
