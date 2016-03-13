class zCThread {
 virtual void ~zCThread(uint);
 virtual void BeginThread();
	virtual void EndThread();
 virtual void IsThreadRunning();
 virtual void SuspendThread();
 virtual void ResumeThread();
 virtual void GetTerminationRequested();
 virtual void SetTerminationRequested(int);
 virtual void ThreadProc();
};
