#include <string>
#include <System/System.h>

#include <cstdio>

void put(std::string s)
{
	fwrite(s.c_str(), s.size(), 1, stdout);
}

void put(char const* w)
{
	put(std::string(w));
}

template<typename T>
void put(T t)
{
	put(std::to_string(t));
}

template<typename... Args>
void print(Args... args)
{
	( put(args), ... );
}

template<typename... Args>
void println(Args... args)
{
	( put(args), ..., put("\n"));
}


void*& sysInstLock = *reinterpret_cast<void**>(0x8D426C);

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
}

#include <chrono>
std::chrono::steady_clock::time_point startTime;
int sysGetTime()
{
	auto now = std::chrono::steady_clock::now();
	std::chrono::milliseconds elapsed = now - startTime;
	return elapsed.count();
}

void InitFunctions()
{
	*reinterpret_cast<void**>(0x5021E8) = (void*)sysAlloc;
	*reinterpret_cast<void**>(0x502A47) = (void*)sysReAlloc;
	*reinterpret_cast<void**>(0x502A97) = (void*)sysFree;
	*reinterpret_cast<void**>(0x505288) = (void*)sysGetTime;
}*/

#include <aw/utility/string/case.h>

#include <System/Win32.h>
extern void InitWin32Stuff(char const* cmdLine);
extern void InitCommonControls();

#include <Game/CGameManager.h>
#include <Debug/zERROR.h>
#include <Debug/zExceptionHandler.h>


void GameLoop();
char(&sysLogName)[260] = Value<char[260]>(0x8D3A90);
extern "C" void aw_main(void* hinst, char const* args)
{
	freopen("log.txt", "wb", stdout);

	print("-- LIBRARY LOADED --\n");

	InitCommonControls();

	hInstApp = hinst;

	std::string cmdLine{args};
	aw::string::toupper(cmdLine);
	zerr.Init(cmdLine);

	bool handler = cmdLine.find("ZNOEXHND") == std::string::npos;
	zCExceptionHandler::SetIsActive(handler);

	// TODO: remove k chertyam sobachim
	std::string log = "DemoW.log";
	std::copy(begin(log), end(log), std::begin(sysLogName));

	InitWin32Stuff(cmdLine.c_str());

	GameLoop();

	libExit();
}

void GameLoop()
{
	CGameManager game;

	sysEvent();

	game.Init((void*)0x8D422C);
	game.Run();
	game.Done();
}
