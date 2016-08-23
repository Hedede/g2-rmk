#include <Hook/log.h>

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
void InitFunctions()
{
	constexpr uintptr_t text_start = 0x401000;
	constexpr uintptr_t text_end   = 0x82E000;

	unsigned long prot = PAGE_EXECUTE_READWRITE;
	VirtualProtect((void*)text_start, text_start - text_end, prot, &prot);

	//*reinterpret_cast<void**>(0x5021E8) = (void*)sysAlloc;
	//*reinterpret_cast<void**>(0x502A47) = (void*)sysReAlloc;
	//*reinterpret_cast<void**>(0x502A97) = (void*)sysFree;
	//make_jump((char*)0x505288, (uintptr_t)sysGetTime, x86::eax);

	// restore protection
	VirtualProtect((void*)text_start, text_start - text_end, prot, &prot);
}
