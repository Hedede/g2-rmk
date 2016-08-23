#include <string>

#include <Hook/log.h>


#include <aw/utility/string/case.h>

#include <Gothic/System/System.h>
#include <Gothic/System/Win32.h>
extern void InitWin32Stuff(char const* cmdLine);
extern void InitFunctions();

#include <Gothic/Game/CGameManager.h>
#include <Gothic/Debug/zERROR.h>
#include <Gothic/Debug/zExceptionHandler.h>

void GameLoop();
char(&sysLogName)[260] = Value<char[260]>(0x8D3A90);
extern "C" void aw_main(void* hinst, char const* args)
{
	freopen("log.txt", "wb", stdout);

	print("-- LIBRARY LOADED --\n");

	hInstApp = hinst;

	std::string cmdLine{args};
	println("Command line: ", cmdLine);

	aw::string::toupper(cmdLine);
	zerr.SetTarget(2);
	zerr.SetFilterLevel(3);
	//zerr.SetFilterAuthors("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	bool handler = cmdLine.find("ZNOEXHND") == std::string::npos;
	println("Exception handler is: ", handler ? "active" : "disabled");
	zCExceptionHandler::SetIsActive(handler);

	// TODO: remove k chertyam sobachim
	std::string log = "DemoW.log";
	std::copy(begin(log), end(log), std::begin(sysLogName));

	InitWin32Stuff(cmdLine.c_str());

	InitFunctions();

	GameLoop();

	libExit();
}

void GameLoop()
{
	CGameManager game;

	sysEvent();

	game.Init(hWndApp);
	game.Run();
	game.Done();
}
