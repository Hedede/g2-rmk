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
#include <Gothic/Debug/zERROR.h>

#include <aw/utility/to_string.h>
#include <Logging/Log.h>
int __thiscall zERROR_Report(void*, int type, int id, zSTRING& message, char levelPrio, char flag, int line, char *file, const char *function)
{
	enum zERROR_TYPE {
		zERR_TYPE_OK = 0x0,
		zERR_TYPE_MESSAGE = 0x1,
		zERR_TYPE_WARNING = 0x2,
		zERR_TYPE_FAULT = 0x3,
		zERR_TYPE_FATAL = 0x4,
	};

	std::string src{file};
	std::string msg{message};

	auto last = src.find_last_of("\\/");
	last = src.find_first_not_of("\\/", last);
	src.erase(0, last);
	if (src.empty())
		src = "Gothic";
	src += " [" + aw::to_string(levelPrio) + "]";

	switch(type) {
	default:
	case zERR_TYPE_OK:
	case zERR_TYPE_MESSAGE:
		g2::Log(src, std::string(msg));
		break;
	case zERR_TYPE_WARNING:
		g2::Warning(src, std::string(msg));
		break;
	case zERR_TYPE_FAULT:
		g2::Error(src, std::string(msg));
		break;
	case zERR_TYPE_FATAL:
		g2::Fatal(src, std::string(msg));
		break;
	};
}

#include "Clobber/zThread.h"

//-----------------------------------------------------------------------------
#include <Graphics/FontMan.h>
size_t __thiscall FontMan_Load(g2::FontMan* fm, zSTRING const& str)
{
	return fm->Load( std::string(str) );
}

zCFont* __thiscall FontMan_GetFont(g2::FontMan* fm, size_t idx)
{
	return fm->GetFont(idx);
}

void InitFontMan()
{
	using namespace g2;
	make_jump((char*)0x7882D0, (uintptr_t)FontMan_Load,    x86::eax);
	make_jump((char*)0x7884B0, (uintptr_t)FontMan_GetFont, x86::eax);
}

void InitFunctions()
{
	using namespace g2;
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
	make_jump((char*)0x44C8D0, (uintptr_t)zERROR_Report, x86::eax);

	InitFontMan();

	Log("Clobber", "Restoring memory protection");
	ret = VirtualProtect((void*)text_start, text_length, prot, &prot);
	if (!ret)
		Error("Clobber", "Could not change memory protection! Error:", GetLastError());
}
