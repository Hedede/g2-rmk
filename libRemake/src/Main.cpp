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

#include <Debug/zERROR.h>

void*& hInstApp = *reinterpret_cast<void**>(0x8D4220);
void*& sysInstLock = *reinterpret_cast<void**>(0x8D426C);
char(&sysLogName)[260] = *reinterpret_cast<char*>(0x8D3A90);

unsigned& winMainThreadID = *reinterpret_cast<unsigned*>(0x8D3D34);
void*& winMainThread = *reinterpret_cast<void**>(0x8D3ED0);

#include <new>
void* sysAlloc(size_t bytes)
u
	return ::operator new(bytes);
}

#include <aw/utility/string.h>
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

	winMainThreadID = GetCurrentThreadId();
	void* a = GetCurrentProcess();
	void* b = GetCurrentThread();
	void* c = GetCurrentProcess();
	DuplicateHandle(a, b, c, &winMainThread, 0, 0, 2u);


	exit(0);
}

void GameLoop()
{
	CGameManager game;

	sysEvent();

	game.Init((void*)0x8D422C);
	game.Run();
	game.Done();

	libExit();
}
