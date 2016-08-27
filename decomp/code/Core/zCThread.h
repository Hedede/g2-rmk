struct zCSyncObject {
	virtual ~zCSyncObject() = default;
	virtual int Lock(unsigned timeOutMsec) = 0;
	virtual int Unlock(unsigned) = 0;
};

struct zCCriticalSection : zCSyncObject {
	zCCriticalSection()
	{
		InitializeCriticalSection(&critSect);
	}

	~zCCriticalSection() override
	{
		DeleteCriticalSection(&critSect);
	}

	int Lock(unsigned) override
	{
		EnterCriticalSection(&critSect);
	}

	void Unlock() override
	{
		LeaveCriticalSection(&critSect);
	}

private:
	CRITICAL_SECTION critSect;
};

struct zCMutex : zCSyncObject {
	zCMutex()
	{
		mutex = CreateMutexA(0, 0, 0);
		if ( !mutex )
			zFATAL("D :"); // 190, zThread.cpp
		return v1;
	}

	~zCMutex() override
	{
		if (mutex)
			CloseHandle(mutex);
	}

	int Lock(unsigned long timeOutMsec)
	{
		WaitForSingleObject(mutex, timeOutMsec);
		return 1;
	}

	void Unlock() override
	{
		ReleaseMutex(mutex);
		return 1;
	}
private:
	HANDLE mutex;
};


struct zCThread {
	zCThread() = default;
	virtual ~zCThread()
	{
		EndThread();
	}

	virtual void BeginThread()
	{
		if (threadHandle)
			EndThread();
		threadHandle = _beginthreadex(0, 0, zThreadProc, this, 0, &threadId);
		if (threadHandle <= 0) {
			zFATAL("D: Could not create Thread!"); // 103
		}
		isRunning = 1;
	}

	virtual void EndThread()
	{
		if (threadHandle) {
			terminationRequested = 1;
			ResumeThread();
			WaitForSingleObject(threadHandle);
			CloseHandle(threadHandle);
		}
	}

	virtual bool IsThreadRunning() const
	{
		retrun isRunning;
	}

	virtual void SuspendThread()
	{
		if (!threadSuspended) {
			threadSuspended = 1;
			return SuspendThread(threadHandle) != -1;
		}
		return 0;
	}

	virtual void ResumeThread()
	{
		if (threadSuspended) {
			threadSuspended = 0;
			return ResumeThread(threadHandle) != -1;
		}
		return 0;
	}

	virtual void GetTerminationRequested() const
	{
		return terminationRequested;
	}

	virtual void SetTerminationRequested(zBOOL b)
	{
		terminationRequested = b;
	}

	virtual int ThreadProc()
	{
		return 0;
	}

	void SleepThread(unsigned msec)
	{
		Sleep(msec);
	}

private:
	HANDLE   threadHandle = 0;
	unsigned threadId     = 0;

	zBOOL threadSuspended      = 0;
	zBOOL isRunning            = 0;
	zBOOL terminationRequested = 0;
};

int zThreadProc(zCThread* thread)
{
	return thread->ThreadProc();
}
