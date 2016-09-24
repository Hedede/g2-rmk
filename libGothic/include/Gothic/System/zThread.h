#pragma once
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

	static void  __thiscall SleepThread_thunk(zCThread *, unsigned);

	void* (__thiscall* dtor)(zCThread *, size_t)  = dtor_thunk;
	void (__thiscall* BeginThread)(zCThread *)    = BeginThread_thunk;
	void (__thiscall* EndThread)(zCThread *)      = EndThread_thunk;
	int (__thiscall* IsThreadRunning)(zCThread *) = IsThreadRunning_thunk;
	int (__thiscall* SuspendThread)(zCThread *)   = SuspendThread_thunk;
	int (__thiscall* ResumeThread)(zCThread *)    = ResumeThread_thunk;
	int (__thiscall* GetTerminationRequested)(zCThread *)       = GetTerminationRequested_thunk;
	void (__thiscall* SetTerminationRequested)(zCThread *, int) = SetTerminationRequested_thunk;
	int  (__thiscall* ThreadProc)(zCThread *) = ThreadProc_thunk;
};

extern zCThread_vt mythread_vt;

struct zCThread {
	zCThread() = default;
	~zCThread();

	void BeginThread();
	void EndThread();
	bool IsThreadRunning() const;
	bool SuspendThread();
	bool ResumeThread();
	bool GetTerminationRequested() const;
	void SetTerminationRequested(zBOOL b);
	int ThreadProc();
	void SleepThread(unsigned msec);

// private:
	zCThread_vt* vtab = &mythread_vt;
	void*    threadHandle = 0;
	unsigned threadId     = 0;

	zBOOL threadSuspended      = 0;
	zBOOL isRunning            = 0;
	zBOOL terminationRequested = 0;
};

