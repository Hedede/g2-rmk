class zCMutex {
	zCMutex()
	{
		mutex = CreateMutexA(0, 0, 0);
		if ( !mutex )
			zFATAL("D :"); // 190, zThread.cpp
		return v1;
	}

	virtual ~zCMutex()
	{
		if (mutex)
			CloseHandle(mutex);
	}

	virtual int Lock(unsigned long timeOutMsec)
	{
		WaitForSingleObject(mutex, timeOutMsec);
		return 1;
	}

	virtual void Unlock()
	{
		ReleaseMutex(mutex);
		return 1;
	}
private:
	HANDLE mutex;
};
