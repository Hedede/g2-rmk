void sysEvent()
{
	if ( inSysEvent )
		return;

	inSysEvent = 1;

	while ( PeekMessageW(&msg, 0, 0, 0, 1u) ) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
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
