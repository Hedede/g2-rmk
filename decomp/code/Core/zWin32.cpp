unsigned sysGetTime()
{
	if ( PreciseTimer ) {
		LARGE_INTEGER PerformanceCount;
		QueryPerformanceCounter(&PerformanceCount);
		return = PerformanceCount.QuadPart / pTimeDiv - BeginTime;
	}

	return timeGetTime() - BeginTime;
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

int winMode;
bool wasInFullScreen;
void sysSetWindowMode(int FullScreen, int Show)
{
	tagRECT Rect;
	if ( FullScreen ) {
		if ( winMode == 1 ) {
			GetWindowRect(hWndApp, &Rect);

			iwPos.X = Rect.left;
			iwPos.Y = Rect.top;
			nWidth = Rect.right - Rect.left;
			nHeight = Rect.bottom - Rect.top;
			wasInFullScreen = 1;
		}

		SetMenu(hWndApp, 0);
		SetWindowLongA(hWndApp, -16, 2147483648);
		SetWindowLongA(hWndApp, -20, 8);
		winMode = 2;

		if ( Show ) {
			SetWindowPos(hWndApp, HWND_MESSAGE|0x2, 0, 0, GetSystemMetrics(0), GetSystemMetrics(1), SWP_FRAMECHANGED);
			if ( !IsZoomed(hWndApp) )
				ShowWindow(hWndApp, 5);
		}
	} else {
		if ( winMode == 2 && wasInFullScreen ) {
			ShowWindow(hWndApp, 0);
			MoveWindow(hWndApp, iwPos.X, iwPos.Y, nWidth, nHeight, 1);
			winResizeMainWindow();
			ccRenderCaption(hWndApp, &ccMain);
		}

		SetWindowLongA(hWndApp, GWL_STYLE, WS_BORDER|WS_SYSMENU|WS_DLGFRAME);
		SetWindowLongA(hWndApp, GWL_EXSTYLE, WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE);
		winCreateMenu();
		SetWindowPos(hWndApp, 0, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
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
	SetWindowPos(hWndApp, 0, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSENDCHANGING);
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

		SetWindowPos(hWndApp, 0, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSENDCHANGING);
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

int winDialogPara(uint16_t resID, DLGPROC Proc, LPARAM Param)
{
	int beginTime = sysGetTime();
	if ( byte_89C86C == 5 )
		vidMinimize(0);
	auto dialog = DialogBoxParamA(hInstApp, resID, hWndApp, Proc, Param);
	if ( dialog )
		vidRestore();
	sysTimeGap += sysGetTime() - beginTime;
	return dialog;
}

int winSuspendThreads()
{
	int result = GetCurrentThreadId();
	if ( result != winMainThreadID )
		result = SuspendThread(winMainThread);
	return result;
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

// unnamed struct on debug info
struct {
	int X,Y;
} iwPos;

void winCalcSizes()
{
	winExtraX = 2 * GetSystemMetrics(32) + 5;
	winExtraY = 2 * GetSystemMetrics(33) + 5 + GetSystemMetrics(4);

	if ( !vidHideMenu )
		winExtraY += GetSystemMetrics(15);
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

	winCalcSizes();

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

	WS_DLGFRAME = 0;

	RECT rect;
	iwPos.Y = 0;
	iwPos.X = 0;

	auto res = SystemParametersInfoA(SPI_GETWORKAREA, 0, &rect, 0);
	if ( res && rect.left >= 0 && rect.top >= 0 ) {
		iwPos.X = rect.left;
		iwPos.Y = rect.top;
	}

	nWidth  = winExtraX + 800;
	nHeight = winExtraY + 600;

	/* Completely useless: */
	dword_8D4100 = 0x90000000;
	dword_8D40FC = 0x90000000;
	dword_8D4108 = 0x90000000;
	dword_8D4104 = 0x90000000;

	dword_8D410C = 0;
	dword_8D40F8 = 0;
	/**/

	hWndApp = CreateWindowExA(
	                WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE,
			"DDWndClass",
			APP_NAME,
			WS_BORDER|WS_SYSMENU|WS_DLGFRAME,
	                iwPos.X, iwPos.Y,
			winExtraX + 800, winExtraY + 600,
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


int HandleResize(WPARAM wParam, RECT* rect)
{
	// INCOMPLETE, kinda don't need it
	if ( vidFreeStretch == 1 || VideoW <= 0 || VideoH <= 0 )
		return 1;

	RECT workarea;
	SystemParametersInfoA(SPI_GETWORKAREA, 0, &workarea, 0);
	if ( !vidWindowStretch )
	{
		if ( wParam == WMSZ_LEFT || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_TOPLEFT )
			rect->left = rect->right - winExtraX - VideoW;
		if ( wParam == WMSZ_RIGHT || wParam == WMSZ_BOTTOMRIGHT || wParam == WMSZ_TOPRIGHT )
			rect->right = VideoW + winExtraX + rect->left;
		if ( wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT )
			rect->top = rect->bottom - winExtraY - VideoH;
		if ( wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT )
			rect->bottom = VideoH + winExtraY + rect->top;
		return 1;
	}
	if ( vidWindowStretch == 4 )
		return 1;
	if ( wParam == WMSZ_LEFT || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_TOPLEFT ) {
		auto wl = winExtraX + VideoW * ((rect->right + VideoW / 2 - rect->left - winExtraX) / VideoW);
		for ( i = rect->right - wl; i < workarea.left; wl -= VideoW )
			i += VideoW;
		rect->left = rect->right - wl;
	}
	if ( wParam == WMSZ_RIGHT || wParam == WMSZ_BOTTOMRIGHT || wParam == WMSZ_TOPRIGHT ) {
		auto wr = winExtraX + VideoW * ((rect->right + VideoW / 2 - rect->left - winExtraX) / VideoW);
		for ( j = rect->left + wr; j > workarea.right; wr -= VideoW )
			j -= VideoW;
		rect->right = wr + rect->left;
	}
	if ( wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT ) {
		auto wt = winExtraY + VideoH * ((rect->bottom + VideoH / 2 - rect->top - winExtraY) / VideoH);
		for ( k = rect->bottom - wt; k < workarea.top; wt -= VideoH )
			k += VideoH;
		rect->top = rect->bottom - wt;
	}
	if ( wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT ) {
		auto wb = winExtraY + VideoH * ((rect->bottom + VideoH / 2 - rect->top - winExtraY) / VideoH);
		for ( l = rect->top + wb; l > workarea.bottom; wb -= VideoH )
			l -= VideoH;
		rect->bottom = wb + rect->top;
	}
	auto ww = (rect->right - rect->left - winExtraX) / VideoW;
	auto wh = (rect->bottom - rect->top - winExtraY) / VideoH;
	if ( ww != 1 || wh  != 1 ) {
		if ( winSnapSizes ) {
			while ( VideoW != winSnapSize[i].x | VideoH != winSnapSize[i].y )
			{
				if ( ++i >= winSnapSizes ) {
					winSnapSize[i].x = VideoW;
					winSnapSize[i].y = VideoH;
					if ( winSnapSizes < 0x20 )
						++winSnapSizes;
				}
			}
			winSnapSize[i].w = ww;
			winSnapSize[i].h = wh;
		}
	} else {
		if ( winSnapSizes ) {
			int num = winSnapSizes;
			do {
				if ( VideoW == winSnapSize[i].x && VideoH == winSnapSize[i].y ) {
					if ( i != num - 1 )
						winSnapSize[i].x = winSnapSize[winSnapSizes];
					--num;
				}
				++i;
			} while ( i < winSnapSizes );
			winSnapSizes = num;
			return 1;
		}
	}
}

int AppWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( sysStopProcessingMessages )
		return DefWindowProcA(hWnd, msg, wParam, lParam);

	switch (msg) {
	case WM_INITMENU:
		winCreateMenu();
		return 0;
	case WM_ENTERMENULOOP:
		winInMenu = 1;
		return 0;
	case WM_EXITMENULOOP:
		winInMenu = 0;
		return 0;
	case WM_SIZING:
		HandleResize();
		return 1;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		SetForegroundWindowEx(hWnd);
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
		inCreateMouseMessage(wParam, lParam);
		return 1;

	case WM_SYSCOMMAND:
		switch (wParam & 0xFFF0) {
		case SC_MONITORPOWER:
		case SC_SCREENSAVE:
			return 0;
		case SC_MINIMIZE:
			if ( !vidAllowMinimizeWindow || byte_89C86C != 4 )
				vidMinimize(1);
			return 0;
		case SC_KEYMENU:
			if ( vidDisableMenuKeys )
				return 0;
		default:
			return 1;
		}

	case WM_ERASEBKGND:
		return 1;
	case WM_CREATE:
		ccMain.unk2 = 0;
		ccMain.unk3 = 0;
		ccMain.icon = hIconApp;
		ccMain.unk1 = 7;
		ccMain.unk7 = 1;
		ccMain.unk10 = 0;
		ccRenderCaption(hWnd, &ccMain);
		return 1;
	case WM_CLOSE:
		usrWantQuit = 1;
		return 0;

	case WM_PAINT:
		if ( !zrenderer || zrenderer->Vid_GetScreenMode() )
			return DefWindowProcA(hWnd, Msg, wParam, lParam);
		BeginPaint(hWnd, &Paint);
		EndPaint(hWnd, &Paint);
		return 1;

	case WM_SIZE:
		RECT rect;
		GetClientRect(hWnd, &winWindowSize);
		GetWindowRect(hWnd, &rect);
		if ( rect.right - rect.left != ccMain.width )
		{
			ccRenderCaption(hWnd, &ccMain);
			ccMain.width = rect.right - rect.left;
		}
		if ( byte_89C86C != 4 || IsIconic(hWndApp) || vidBusyMode & 3 || vidWindowStretch != 4 )
			return 0;
		VIDMODE vm = VideoModeCur;
		vm.width  = winWindowSize.right - 1;
		vm.height = winWindowSize.bottom - 1;
		winSetVideoMode(&vm);
		return 0;
	case WM_DESTROY:
		sysKillWindowsKeys(0);
		return 0;
	case WM_DEADCHAR:
		// possibly bug in original code
	case WM_ACTIVATE:
		zINFO(8, "C: WM_ACTIVATE received"); // 1720, _carsten, zWin32.cpp
		HandleFocusLoose();
		return 1;
	case WM_CANCELMODE:
		zINFO(8"C: WM_CANCELMODE received"); //1728,
		HandleFocusLoose();
		return 1;
	case WM_ACTIVATEAPP:
		zINFO(8, "C: WM_ACTIVATEAPP received"); //1738,
		HandleFocusLoose();
		return 1;
	case WM_GETMINMAXINFO:
		if ( VideoW <= 0 || VideoH <= 0 )
			return 1;
		if ( vidWindowStretch != 4 ) {
			lParam->ptMinTrackSize.x = VideoW + winExtraX;
			lParam->ptMinTrackSize.y = VideoH + winExtraY;
		}
		result = 0;
		break;
	case WM_NCPAINT:
	case WM_NCACTIVATE:
		if ( !zrenderer || zrenderer->Vid_GetScreenMode() )
			return DefWindowProcA(hWnd, Msg, wParam, lParam);
		break;
	case WM_SETCURSOR:
		lParam &= 0xFFFF;
		if ( lParam == 1 ) {
			if ( winShowMouse && (winShowMouse != 2 || byte_89C86C != 5) )
			{
				if ( winCursorShowState < 0 )
					winCursorShowState = ShowCursor(1);
				SetCursor(LoadCursorA(0, 0x7F00));
			}
			else
			{
				if ( winCursorShowState >= 0 )
					winCursorShowState = ShowCursor(0);
			}
			return 1;
		} else {
			if ( winCursorShowState < 0 )
				winCursorShowState = ShowCursor(1);
			HCURSOR cursor;
			switch ( lParam ) {
			case 0xF:
			case 0xC:
				cursor = LoadCursorA(0, 0x7F85);
				break;
			case 0xA:
			case 0xB:
				cursor = LoadCursorA(0, 0x7F84);
				break;
			case 0xD:
			case 0x11:
				cursor = LoadCursorA(0, 0x7F82);
				break;
			case 0xE:
			case 0x10:
				cursor = LoadCursorA(0, 0x7F83);
				break;
			default:
				cursor = LoadCursorA(0, 0x7F00);
				break;
			}
			SetCursor(cursor);
			return 1;
		}
		break;

	case WM_COMMAND:
		if ( in(wParam, 41000, 41001, 41003) ) {
			TRACE("WIN: Switch to bit depth %u BPP", 8 * wParam - 327992);
			VIDMODE vm = VideoModeCur;
			vm.bpp = 8 * wParam - 56;
			vm.unk1 = 0;
			winSetVideoMode(&vm);
			return 1;
		}

		if ( wParam >= 41010 && wParam <= 41109 ) {
			if ( winSetVideoMode ) {
				VIDMODE vm;
				// some kind of bullshit, maybe screwed up binary patch?
				unsigned val = "X: DirectDraw out of sync !"[4*wParam + 12];
				vm.width = val
				vm. = (val >> 28) & 0xF;
				vm.height = (val >> 16) & 0xFFF;
				winSetVideoMode(&vm);
			}
			return 1;
		}
		if ( wParam >= 40400 && wParam < 40656 ) {
			auto menu = ENV_STDMENU[wParam - 40400];
			// no idea what this is, doesn't match MENU's in winapi docs
			switch ( menu ) {
			case 0x46:
				usrWantPause ^= 4u;
				break;
			case 0x45:
				usrWantQuit = 3;
				break;
			case 0x21:
				(*(menu + 2))(*(menu + 3));
				break;
			case 0x22:
				**(menu + 2) = **(menu + 2);
				break;
			case 0x23:
				**(menu + 2) ^= *(menu + 12);
				break;
			case 0x24:
				**(menu + 2) ^= *(menu + 3);
				break;
			case 0x25:
				**(menu + 2) = *(menu + 3);
				break;
			case 0x33:
				vidFreeStretch ^= 1u;
				if ( vidFreeStretch )
					return 1;
				winResizeMainWindow();
				break;
			}
			return 1;
		}
		return 1;
	}

	default:
		static auto WM_QUERY_CANCEL_AUTOPLAY = RegisterWindowMessageA("QueryCancelAutoPlay");
		if ( msg == WM_QUERY_CANCEL_AUTOPLAY )
			return 1;
		return DefWindowProcA(hWnd, msg, wParam, lParam);
	}
	return 1;
	}

BOOL __stdcall SplashDialogProc(HWND, UINT msg, WPARAM, LPARAM)
{
	if ( msg == WM_DESTROY )
		PostQuitMessage(0);
	return 0;
}

DWORD WINAPI SplashThreadProc(LPVOID lpThreadParameter)
{
	SplashHwnd = CreateDialogParamA(hSplashInstance, 0xA5, 0, SplashDialogProc, 0);
	if ( SplashHwnd ) {
		if ( auto it = GetDlgItem(SplashHwnd, 0x436) ) {
			cy = splash_cy;
			cx = splash_cx;
			auto x = (GetSystemMetrics(0) - splash_cx) / 2;
			auto y = (GetSystemMetrics(1) - splash_cy) / 2;
			SetWindowPos(SplashHwnd, HWND_MESSAGE|0x2, x, y, cx, cy, SWP_SHOWWINDOW);
			SetWindowPos(SplashHwnd, 0xFFFFFFFE, 0, 0, 0, 0, 3u);
			SendMessageA(it, 0x172u, 0, SplashBitmap);
			UpdateWindow(SplashHwnd);

			MSS msg;
			while ( GetMessageA(&msg, 0, 0, 0) )
				DispatchMessageA(&msg);
		}
	}
	return 0;
}

const char *vidGetLastError()
{
	return vidLastError ? vidLastError : "No error";
}

BOOL vidIsMinimized()
{
	return IsIconic(hWndApp);
}

