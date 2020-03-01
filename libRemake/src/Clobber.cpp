#include <cassert>
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
int __thiscall zERROR_Report(void*, int type, int id, zSTRING const& message, char levelPrio, char flag, int line, char *file, const char *function)
{
	enum zERROR_TYPE {
		zERR_TYPE_OK = 0x0,
		zERR_TYPE_MESSAGE = 0x1,
		zERR_TYPE_WARNING = 0x2,
		zERR_TYPE_FAULT = 0x3,
		zERR_TYPE_FATAL = 0x4,
	};

	std::string src{file};
	char const* wtf = message.Data();
	std::string msg{wtf ? wtf : ""};

	auto last = src.find_last_of("\\/");
	last = src.find_first_not_of("\\/", last);
	src.erase(0, last);
	if (src.empty())
		src = "Gothic";
	src += " [" + aw::to_string((int)levelPrio) + "]";

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
	as::jump_rel((char*)0x7882D0, (uintptr_t)FontMan_Load);
	as::jump_rel((char*)0x7884B0, (uintptr_t)FontMan_GetFont);
}

void InitThread()
{
	as::jump_rel((char*)0x5F9370, (uintptr_t)zCThread_vt::SuspendThread_thunk);
	as::jump_rel((char*)0x5F93A0, (uintptr_t)zCThread_vt::ResumeThread_thunk);
	as::jump_rel((char*)0x5F93D0, (uintptr_t)zCThread_vt::SleepThread_thunk);
	as::jump_rel((char*)0x5F9230, (uintptr_t)zCThread_vt::BeginThread_thunk);
	as::jump_rel((char*)0x5F9330, (uintptr_t)zCThread_vt::EndThread_thunk);
	as::jump_rel((char*)0x5F9180, (uintptr_t)zCThread_vt::dtor_thunk);
	// stubify ~zCThread(),
	// so that resorcemanager won't call dtor twice
	as::retn((char*)0x5F91E0);
}

void InitStdc()
{
	as::jump_rel((char*)0x7B4460, (uintptr_t)malloc);
	as::jump_rel((char*)0x7B4465, (uintptr_t)calloc);
	as::jump_rel((char*)0x7B446A, (uintptr_t)realloc);
	as::jump_rel((char*)0x7B446F, (uintptr_t)free);
	as::jump_rel((char*)0x7CFF47, (uintptr_t)atexit);
}

#include <Gothic/Camera/zMovementTracker.h>

uintptr_t copy_function(uintptr_t begin, uintptr_t end);
uintptr_t original_GetLastValidWayPoint;
bool yabzur = 0;
zVEC3& __thiscall thunk_GetLastValidWayPoint(zCMovementTracker* tracker, int const& type)
{
	using T = zCMovementTracker::zTWayPoint;
	Thiscall<zVEC3&(zCMovementTracker*, T const&)> orig{original_GetLastValidWayPoint};
	auto& v1 = orig( tracker, T(type) );
	auto& v2 = tracker->GetLastValidWayPoint( T(type) );
	if (&v1 != &v2){
		yabzur = 1;
		g2::Log("ZMOVTRAC", "Type: ", type, " Adresses: ", uintptr_t(&v1),' ',uintptr_t(&v2));
	}
	auto look1 = orig( tracker, T(type) );
	auto look2 = tracker->GetLastValidWayPoint( T(type) );
	yabzur = 0;
	return v2;
}
void InitMisc()
{
	as::jump_rel((char*)0x44C8D0, (uintptr_t)zERROR_Report);

	g2::Log("Clobber", "Clobbering GetLastValidWayPoint" );
	original_GetLastValidWayPoint = copy_function(0x4B81B0, 0x4B8389);
	as::jump_rel((char*)0x4B81B0, (uintptr_t)thunk_GetLastValidWayPoint);
}

void InitConsole()
{
	void Game_InitConsole();

	g2::Log("Clobber", "Clobbering Game_InitConsole" );
	as::jump_rel((char*)0x6D01F0, (uintptr_t)Game_InitConsole);
}

#include <algorithm>
#include <Gothic/Graphics/zSkyControler.h>
#include <stdlib.h>
void InitFunctions()
{
	using namespace g2;
	constexpr uintptr_t text_start = 0x401000;
	constexpr uintptr_t text_end   = 0x82E000;
	constexpr auto text_length = text_end - text_start;

	Log("Clobber", "Making .text segment writeable");
	DWORD prot = PAGE_EXECUTE_READWRITE;
	bool ret;
	ret = VirtualProtect((void*)text_start, text_length, prot, &prot);
	if (!ret)
		Error("Clobber", "Could not change memory protection! Error:", GetLastError());

	//*reinterpret_cast<void**>(0x5021E8) = (void*)sysAlloc;
	//*reinterpret_cast<void**>(0x502A47) = (void*)sysReAlloc;
	//*reinterpret_cast<void**>(0x502A97) = (void*)sysFree;
	//make_jump((char*)0x505288, (uintptr_t)sysGetTime, x86::eax);


	InitFontMan();
	InitThread();
	InitMisc();
	InitStdc();
	InitConsole();

	Log("Clobber", "Restoring memory protection");
	ret = VirtualProtect((void*)text_start, text_length, prot, &prot);
	if (!ret)
		Error("Clobber", "Could not change memory protection! Error:", GetLastError());
}

uintptr_t copy_function(uintptr_t begin, uintptr_t end)
{
	assert( begin < end );

	size_t count = end-begin;

	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	auto const page_size = system_info.dwPageSize;

	g2::Log("Clobber", "Copying function ", begin );
	auto const memory = VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_READWRITE);
	std::memcpy(memory, (const char*)begin, count);

	g2::Log("Clobber", "Making function executable" );
	DWORD prot = PAGE_EXECUTE_READ;
	VirtualProtect(memory, count, prot, &prot);

	return uintptr_t(memory);
}
