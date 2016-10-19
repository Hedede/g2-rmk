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

	using InfoCallback = void(*)(EXCEPTION_DESCR *);
	static void RemoveUnhandledExceptionInfoCallback(InfoCallback cb)
	{
		ExceptionInfoCallbackList.Remove(cb);
	}

	static void AddUnhandledExceptionInfoCallback(InfoCallback cb)
	{
		ExceptionInfoCallbackList.Insert(cb);
	}

	using ReleaseCallback = void(*)();
	static void RemoveUnhandledExceptionReleaseCallback(ReleaseCallback cb)
	{
		ExceptionReleaseCallbackList.Remove(cb);
	}

	static void AddUnhandledExceptionReleaseCallback(ReleaseCallback cb)
	{
		ExceptionReleaseCallbackList.Insert(cb);
	}

private:
	static zBOOL isActive;
	static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
	static char* m_szLogFileName;
	static zCArray<ReleaseCallback> ExceptionReleaseCallbackList;
	static zCArray<InfoCallback>    ExceptionInfoCallbackList;
};

zBOOL zExHandler;
