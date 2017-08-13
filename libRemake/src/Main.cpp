#include <string>

#include <Logging/Log.h>
#include <Logging/LogFile.h>

#include <aw/utility/string/case.h>

#include <Gothic/System/System.h>
#include <Gothic/System/Win32.h>
extern void InitWin32Stuff(char const* cmdLine);
extern void InitFunctions();

#include <Gothic/Game/CGameManager.h>
#include <Gothic/Debug/zERROR.h>
#include <Gothic/Debug/zExceptionHandler.h>

void GameLoop();

extern "C" void __attribute__((dllexport)) aw_main(void* hinst, char const* args)
{
	using namespace g2;
	LogFile file_log;
	logger.set_logger(&file_log);

	Log("Main", "Successfully hooked.");

	hInstApp = hinst;

	std::string cmdLine{args};
	Log("Main", "Command line: ", cmdLine);

	aw::string::toupper(cmdLine);
	zerr.SetTarget(2);
	zerr.SetFilterLevel(10);

	bool handler = cmdLine.find("ZNOEXHND") == std::string::npos;
	Log("Main", "Exception handler is ", handler ? "active" : "disabled");
	zCExceptionHandler::SetIsActive(handler);

	InitWin32Stuff(cmdLine.data());

	auto& sysCommandLine = Value<char*>(0x8D3D2C);
	sysCommandLine = cmdLine.data();

	InitFunctions();

	GameLoop();

	libExit();
}


void GameLoop()
{
	using namespace g2;
	Log("GameLoop", "Constructing CGameManager");
	CGameManager game;

	sysEvent();

	Log("GameLoop", "Gameloop starting.");

	game.Init(hWndApp);
	game.PreRun();
	game.Run();
	game.Done();
}
