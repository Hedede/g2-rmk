void ASSERT_FAIL(char *assertion, char *sourceFile, int line, zSTRING const& message)
{
	char *v4; // esi@1
	DWORD v5; // eax@8
	int v6; // esi@10
	DWORD v7; // eax@11
	LARGE_INTEGER v8; // [sp+Ch] [bp-374h]@11
	LARGE_INTEGER PerformanceCount; // [sp+14h] [bp-36Ch]@8
	char v10; // [sp+1Ch] [bp-364h]@7
	int v11; // [sp+348h] [bp-38h]@7
	int v12; // [sp+34Ch] [bp-34h]@7
	 dwInitParam; // [sp+350h] [bp-30h]@3
	 ms_exc; // [sp+368h] [bp-18h]@7

	char const* reason = message.Data();

	LPARAM dwInitParam = "Assertion Failed";


	if ( !sysExiting ) {
		sysExiting = 1;

		TRACE("WIN: Assertion Failed!");
		TRACE(EMPTYSTR);
		TRACE("WIN: Assertion : ASSERT(%s)",   assertion);
		TRACE("WIN: File      : %s (line %u)", sourceFile, line);

		if (reason)
			TRACE("WIN: Reason    : %s", reason);

		TRACE(EMPTYSTR);

		auto tmp = zCExceptionHandler::GetIsActive();
		zCExceptionHandler::SetIsActive(0);

		CPPEH_RECORD ms_exc;
		ms_exc.registration.TryLevel = -1;

		// lea     edx, [ebp+var_364]
		// push    edx             ; struct EXCEPTION_INFO *
		// lea     eax, [ebp+dwInitParam]
		// push    eax             ; struct ASSERT_FAIL_INFO *
		// mov     ecx, [ebp+ms_exc.exc_ptr]
		// push    ecx             ; struct _EXCEPTION_POINTERS *
		// zCExceptionHandler::AssertExceptionFilter(ms_exc.exc_ptr, dwInitParam, );

		zCExceptionHandler::SetIsActive(tmp);

		AssertSpyInfo(&dwInitParam, &v10);
		if ( PreciseTimer )
		{
			QueryPerformanceCounter(&PerformanceCount);
			PerformanceCount.LowPart = PerformanceCount.QuadPart / pTimeDiv;
			v5 = PerformanceCount.LowPart;
		}
		else
		{
			v5 = timeGetTime();
		}
		v6 = v5 - BeginTime;
		vidCloseDisplay(0);
		ShowWindow(hWndApp, 0);
		DialogBoxParamA(hInstApp, 0x94, 0, ASSERTDlgProc, &dwInitParam);
		if ( PreciseTimer )
		{
			QueryPerformanceCounter(&v8);
			v8.LowPart = v8.QuadPart / pTimeDiv;
			v7 = v8.LowPart;
		}
		else
		{
			v7 = timeGetTime();
		}
		sysTimeGap += v7 - BeginTime - v6;
		libExit();
		if ( winMainThread )
			CloseHandle(winMainThread);
		if ( hHandle )
			CloseHandle(hHandle);
		ExitProcess(0);
	}
	TRACE("WIN: Warning! ASSERT(%s) Failed in %s (line %u)", assertion, sourceFile, line);
}
