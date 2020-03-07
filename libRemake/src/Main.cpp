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
#include <aw/fileformat/wav/log.h>

void GameLoop();
void aw_main(char const* args);
extern "C" __attribute__((dllimport)) char* __stdcall GetCommandLineA();

extern "C" void __attribute__((dllexport)) aw_main_crt_startup(void* hinst)
{
	using namespace g2;

	LogFile file_log;
	logger.set_logger(&file_log);
	aw::wav::log.set_logger(&file_log);

	Log("Main", "aw_main_crt_startup");
	hInstApp = hinst;

	InitFunctions();

	Cdecl<void()> mtinit{0x7D622C};
	Cdecl<void()> ioinit{0x7D4D6B};
	Cdecl<void()> cinit{0x7D128D};
	Cdecl<char*()> wincmdln{0x7DB491};

	mtinit();
	ioinit();
	cinit();

	auto& acmdln = Value<char*>(0xABB8A4);
	acmdln = GetCommandLineA();
	auto cline = wincmdln();
	Log("Main", "GetCommandLineA:");
	Log("Main", acmdln);
	Log("Main", "wincmdln:");
	Log("Main", cline);
	aw_main(cline);
}

void aw_main(char const* args)
{
	using namespace g2;

	Log("Main", "Successfully hooked.");
	
	if (!args)
		args = "";

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

	GameLoop();

	libExit();
}


#include <windows.h>
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
