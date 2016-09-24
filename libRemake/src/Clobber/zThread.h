#include <Gothic/System/zThread.h>
#include <process.h>

zCThread_vt mythread_vt;

unsigned __stdcall zThreadProc(void* thread);
zCThread::~zCThread()
{
	EndThread();
}

void zCThread::BeginThread()
{
	if (threadHandle)
		EndThread();
	threadHandle = (void*)_beginthreadex(0, 0, zThreadProc, this, 0, &threadId);
	if (threadHandle <= 0) {
		exit(1);
	}
	isRunning = 1;
}

void zCThread::EndThread()
{
	if (threadHandle) {
		terminationRequested = 1;
		ResumeThread();
		WaitForSingleObject(threadHandle, -1);
		CloseHandle(threadHandle);
	}
}

bool zCThread::IsThreadRunning() const
{
	return isRunning;
}

bool zCThread::SuspendThread()
{
	if (!threadSuspended) {
		g2::Log("zThread", "Suspending thread:", uintptr_t(this));
		threadSuspended = 1;
		return ::SuspendThread(threadHandle) != -1;
	}
	return 0;
}

bool zCThread::ResumeThread()
{
	if (threadSuspended) {
		g2::Log("zThread", "Resuming thread:", uintptr_t(this));
		threadSuspended = 0;
		return ::ResumeThread(threadHandle) != -1;
	}
	return 0;
}

bool zCThread::GetTerminationRequested() const
{
	return terminationRequested;
}

void zCThread::SetTerminationRequested(zBOOL b)
{
	terminationRequested = b;
}

int zCThread::ThreadProc()
{
	return 0;
}

void zCThread::SleepThread(unsigned msec)
{
	Sleep(msec);
}

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
void __thiscall zCThread_vt::SleepThread_thunk(zCThread* self, unsigned ms)
{
	self->SleepThread(ms);
}
