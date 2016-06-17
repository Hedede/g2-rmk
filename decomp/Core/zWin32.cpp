unsigned sysGetTime()
{
	if ( PreciseTimer ) {
		LARGE_INTEGER PerformanceCount;
		QueryPerformanceCounter(&PerformanceCount);
		return = PerformanceCount.QuadPart / pTimeDiv - BeginTime;
	}

	return = timeGetTime() - BeginTime;
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
	int v1; // esi@6
	bool v2; // bl@28
	char v3; // bl@29
	char v4; // [sp+13h] [bp-6Dh]@22
	zSTRING v6; // [sp+1Ch] [bp-64h]@28
	zSTRING section; // [sp+30h] [bp-50h]@27
	zSTRING msg; // [sp+44h] [bp-3Ch]@22
	struct tagMSG Msg; // [sp+58h] [bp-28h]@10
	int v10; // [sp+7Ch] [bp-4h]@22

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
