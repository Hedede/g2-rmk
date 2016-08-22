struct zCExceptionHandler {
	zCExceptionHandler() = default;
	~zCExceptionHandler()
	{
		SetUnhandledExceptionFilter(m_previousFilter);
	}

	int SetLogFileName(char* logFileName)
	{
		strcpy(m_szLogFileName, logFileName);
		return result;
	}

	static void SetIsActive(int b)
	{
		if ( b ) {
			SetCrashHandlerFilter(UnhandledExceptionFilter);
			isActive = b;
		} else {
			SetCrashHandlerFilter(0);
			isActive = 0;
		}
	}

	static zBOOL GetIsActive()
	{
		return isActive
	}

	static void WalkReleaseCallbacks()
	{
		sysKillWindowsKeys(0);

		for (auto& cb : ExceptionReleaseCallbackList)
			cb();

		ExceptionReleaseCallbackList.DeleteList();
	}

private:
	static zBOOL isActive;
	static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
	static char* m_szLogFileName;
	static zCArray<void(*)(EXCEPTION_DESCR *)> ExceptionReleaseCallbackList;
};

zBOOL zExHandler;
