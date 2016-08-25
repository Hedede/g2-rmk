#include <Logging/Log.h>

/*
#include <new>
void* sysAlloc(size_t bytes)
u
	return ::operator new(bytes);
}

#include <cstdlib>
void* sysAlloc(size_t bytes)
{
	return std::malloc(bytes);
}

void* sysFree(void* mem)
{
	return std::free(mem);
}

void* sysReAlloc(void* mem, size_t bytes)
{
	if (mem)
		return std::malloc(bytes);
	return std::realloc(mem, bytes);
}*/

#include <chrono>
int sysGetTime()
{
	using namespace std::chrono;

	static auto startTime = steady_clock::now();
	auto now = steady_clock::now();
	auto elapsed = duration_cast<milliseconds>(now - startTime);
	return elapsed.count();
}

#include <Hook/clobber.h>

#define _WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Thread/zThread.h"

void InitFunctions()
{
	using namespace g2r;
	constexpr uintptr_t text_start = 0x401000;
	constexpr uintptr_t text_end   = 0x82E000;
	constexpr auto text_length = text_end - text_start;

	Log("Clobber", "Making .text segment writeable");
	unsigned long prot = PAGE_EXECUTE_READWRITE;
	bool ret;
	ret = VirtualProtect((void*)text_start, text_length, prot, &prot);
	if (!ret)
		Error("Clobber", "Could not change memory protection! Error:", GetLastError());

	//*reinterpret_cast<void**>(0x5021E8) = (void*)sysAlloc;
	//*reinterpret_cast<void**>(0x502A47) = (void*)sysReAlloc;
	//*reinterpret_cast<void**>(0x502A97) = (void*)sysFree;
	//make_jump((char*)0x505288, (uintptr_t)sysGetTime, x86::eax);

	make_jump((char*)0x5F9370, (uintptr_t)zCThread_vt::SuspendThread_thunk, x86::eax);
	make_jump((char*)0x5F93A0, (uintptr_t)zCThread_vt::ResumeThread_thunk, x86::eax);

	Log("Clobber", "-- Restoring memory protection --");
	ret = VirtualProtect((void*)text_start, text_length, prot, &prot);
	if (!ret)
		Error("Clobber", "Could not change memory protection! Error:", GetLastError());
}
