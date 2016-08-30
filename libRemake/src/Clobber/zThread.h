#include <Gothic/Types/Base.h>
struct zCThread;
struct zCThread_vt {
	static void* __thiscall dtor_thunk(zCThread *, size_t);
	static void  __thiscall BeginThread_thunk(zCThread*);
	static void  __thiscall EndThread_thunk(zCThread*);
	static int   __thiscall IsThreadRunning_thunk(zCThread*);
	static int   __thiscall SuspendThread_thunk(zCThread *);
	static int   __thiscall ResumeThread_thunk(zCThread *);
	static int   __thiscall GetTerminationRequested_thunk(zCThread *);
	static void  __thiscall SetTerminationRequested_thunk(zCThread *, int);
	static int   __thiscall ThreadProc_thunk(zCThread *);

	void* (__thiscall* dtor)(zCThread *, size_t)  = dtor_thunk;
	void (__thiscall* BeginThread)(zCThread *) = BeginThread_thunk;
	void (__thiscall* EndThread)(zCThread *)   = EndThread_thunk;
	int (__thiscall* IsThreadRunning)(zCThread *) = IsThreadRunning_thunk;
	int (__thiscall* SuspendThread)(zCThread *) = SuspendThread_thunk;
	int (__thiscall* ResumeThread)(zCThread *)  = ResumeThread_thunk;
	int (__thiscall* GetTerminationRequested)(zCThread *)       = GetTerminationRequested_thunk;
	void (__thiscall* SetTerminationRequested)(zCThread *, int) = SetTerminationRequested_thunk;
	int  (__thiscall* ThreadProc)(zCThread *) = ThreadProc_thunk;
};

#include <process.h>
unsigned __stdcall zThreadProc(void* thread);
struct zCThread {
	zCThread() = default;
	~zCThread()
	{
		EndThread();
	}

	void BeginThread()
	{
		if (threadHandle)
			EndThread();
		threadHandle = (void*)_beginthreadex(0, 0, zThreadProc, this, 0, &threadId);
		if (threadHandle <= 0) {
			exit(1);
		}
		isRunning = 1;
	}

	void EndThread()
	{
		if (threadHandle) {
			terminationRequested = 1;
			ResumeThread();
			WaitForSingleObject(threadHandle, -1);
			CloseHandle(threadHandle);
		}
	}

	bool IsThreadRunning() const
	{
		return isRunning;
	}

	bool SuspendThread()
	{
		if (!threadSuspended) {
			g2r::Log("zThread", "Suspending thread:", uintptr_t(this));
			threadSuspended = 1;
			return ::SuspendThread(threadHandle) != -1;
		}
		return 0;
	}

	bool ResumeThread()
	{
		if (threadSuspended) {
			g2r::Log("zThread", "Resuming thread:", uintptr_t(this));
			threadSuspended = 0;
			return ::ResumeThread(threadHandle) != -1;
		}
		return 0;
	}

	bool GetTerminationRequested() const
	{
		return terminationRequested;
	}

	void SetTerminationRequested(zBOOL b)
	{
		terminationRequested = b;
	}

	int ThreadProc()
	{
		return 0;
	}

	void SleepThread(unsigned msec)
	{
		Sleep(msec);
	}

private:
	zCThread_vt* vtab;
	void*    threadHandle = 0;
	unsigned threadId     = 0;

	zBOOL threadSuspended      = 0;
	zBOOL isRunning            = 0;
	zBOOL terminationRequested = 0;
};

unsigned __stdcall zThreadProc(void* thread)
{
	return reinterpret_cast<zCThread*>(thread)->ThreadProc();
}

void* __thiscall zCThread_vt::dtor_thunk(zCThread* self, size_t d)
{
	if (self)
		self->~zCThread();
	if (d & 1)
		operator delete(self);
	return self;
}
void __thiscall zCThread_vt::BeginThread_thunk(zCThread* self)
{
	self->BeginThread();
}
void __thiscall zCThread_vt::EndThread_thunk(zCThread* self)
{
	self->EndThread();
}
int __thiscall zCThread_vt::IsThreadRunning_thunk(zCThread* self)
{
	return self->IsThreadRunning();
}
int __thiscall zCThread_vt::SuspendThread_thunk(zCThread* self)
{
	return self->SuspendThread();
}
int __thiscall zCThread_vt::ResumeThread_thunk(zCThread* self)
{
	return self->ResumeThread();
}
int __thiscall zCThread_vt::GetTerminationRequested_thunk(zCThread* self)
{
	return self->GetTerminationRequested();
}
void __thiscall zCThread_vt::SetTerminationRequested_thunk(zCThread* self, int b)
{
	self->SetTerminationRequested(b);
}
int  __thiscall zCThread_vt::ThreadProc_thunk(zCThread* self)
{
	return self->ThreadProc();
}
