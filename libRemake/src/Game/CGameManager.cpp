#include <Gothic/Game/CGameManager.h>
#include <Gothic/Game/zOptions.h>
#include <Gothic/Game/oSaveGame.h>
#include <Gothic/Audio/zMusic.h>
#include <Gothic/Audio/zSound.h>
#include <Gothic/Menu/zMenu.h>
#include <Gothic/System/System.h>

namespace g2 {
void InitSound()
{
	Log("Startup", "Initializing Sound");
	float masterVol = zoptions->ReadReal("SOUND", "soundVolume", 1.0);
	if ( zsound )
		zsound->SetMasterVolume(masterVol);
}

auto ChangeMusicEnabled = func<zCOptions::ChangeHandler>(0x42D240);
void InitMusic()
{
	Log("Startup", "Initializing Music");
	zoptions->InsertChangeHandler("SOUND", "musicEnabled", ChangeMusicEnabled);
	float musicVol = zoptions->ReadReal("SOUND", "musicVolume", 0.8);
	if ( zmusic )
		zmusic->SetVolume(musicVol);
}

auto DefineMenuScriptFunctions = Cdecl<int()>(0x42C1D0);
void InitMenu()
{
	Log("Startup", "Initializing the menu-system");
	zoptions->ReadBool("INTERNAL", "extendedMenu" , 0);
	zCMenu::CreateParser();
	DefineMenuScriptFunctions();
}

void InitOptions()
{
	zoptions->ReadInt("PERFORMANCE", "sightValue" , 4);
	zoptions->ReadReal("PERFORMANCE", "modelDetail" , 0.5);
	zoptions->ReadReal("INTERNAL", "texDetailIndex" , 0.6);
	zoptions->ReadInt("GAME", "skyEffects" , 1);
	zoptions->ReadInt("GAME", "bloodDetail" , 2);
	zoptions->ReadReal("VIDEO", "zVidBrightness" , 0.5);
	zoptions->ReadReal("VIDEO", "zVidContrast"   , 0.5);
	zoptions->ReadReal("VIDEO", "zVidGamma"      , 0.5);
}

void InitGameOptions();

void InitConsole();
} // namespace g2



#include <SDL2/SDL.h>
void CGameManager::Init(void* hwnd)
{
	using namespace g2;

	sysContextHandle = hwnd;

	Log("Startup", "Initializing GameManager");

	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);

	sysEvent();

	PreGraphicsInit();

	sysEvent();


	savegameManager = new oCSavegameManager();
	savegameManager->Reinit();

	sysEvent();

	g2::InitSound();
	g2::InitMusic();
	g2::InitMenu();
	g2::InitOptions();


	if ( zoptions->Parm("PLAYER") ) {
		std::string playerInst = zoptions->ParmValue("PLAYER");
		if ( zgameoptions )
			zgameoptions->WriteString("SETTINGS", "Player", playerInst, 1);
		else
			zoptions->WriteString("INTERNAL", "playerInstanceName", playerInst, 1);
	}

	sysEvent();
	SetEnableHandleEvent(true);

	GameSessionInit();

	sysEvent();

	// TODO: move into InitMenu
	zCMenu::Startup();

	Log("Startup", "GameManager initializing done");
}


#include <Graphics/SplashWindow.h>
#include <Gothic/System/zFILE_VDFS.h>
#include <Gothic/Graphics/zTexture.h>
#include <Gothic/Game/oObjectFactory.h>
#include <Gothic/Game/zResourceManager.h>
#include <Gothic/Menu/zView.h>
#include <Graphics/FontMan.h>
#include <Gothic/Script/zParser.h>
void CGameManager::PreGraphicsInit()
{
	using namespace g2;

	SplashWindow splash;

	if (!splash.IsRunning())
		Warning("Startup", "Could not create splash window.");

	zCClassDef::EndStartup();

	if ( zfactory ) {
		Warning("Startup", "zfactory already exists?");
		zfactory->Release();
	}
	zfactory = new oCObjectFactory;

	Cdecl<void()> zInitOptions{0x4701F0};
	zInitOptions();

	bool memProfiler = zoptions->Parm("ZMEMPROFILER");
	//stub in .exe
	//zCMallocGeneric::Init(memProfiler);

	zFILE_VDFS::InitFileSystem();

	sysEvent();

	bool noResThread = zoptions->Parm("ZNORESTHREAD");

	zresMan = new zCResourceManager;
	zresMan->SetThreadingEnabled(!noResThread);

	zoptions->ChangeDir(DIR_SYSTEM);
	//_control87(0x9001Fu, 0xFFFFu);

	zfontman = new g2::FontMan{};

	g2::InitConsole();

	zCParser::enableParsing = zoptions->Parm("ZREPARSE");

	sysEvent();

	Cdecl<void(void*&)> zDieter_StartUp{0x630580};
	zDieter_StartUp(sysContextHandle);
	Cdecl<void(void*&)> zCarsten_StartUp{0x509580};
	zCarsten_StartUp(sysContextHandle);

	bool texConvert = zoptions->Parm("ZTEXCONVERT");
	if ( texConvert ) {
		std::string val = zoptions->ParmValue("ZTEXCONVERT");
		zCTexture::ScanConvertTextures(val);
	}

	// zfpuControler->RestoreDefaultControlWord()
	// zfpuControler->SaveCurrentControlWord();
	
	if (zoptions->ReadBool("GAME", "playLogoVideos", 1)) {
		Log("Startup", "Playing videos");
		PlayVideo("logo1.bik");
		PlayVideo("logo2.bik");
	}

	sysEvent();

	bool recalc = zoptions->ReadBool("PERFORMANCE", "recalc", 1);
	zoptions->WriteBool("PERFORMANCE", "recalc", 0, 0);

	if ( recalc )
		InitSettings();

	InitScreen_Open();
	g2::InitGameOptions();

	vidScreen = screen;
	vidScreen->SetEnableHandleEvent(1);

	sysEvent();

	Cdecl<void()> oCarsten_StartUp{0x4816F0};
	oCarsten_StartUp();

	zCMenu::EnterCallback = func<zCMenu::Callback>(0x426A80); // MenuEnterCallback;
	zCMenu::LeaveCallback = func<zCMenu::Callback>(0x426B80); // MenuLeaveCallback;

	bool convertAll = zoptions->Parm("ZCONVERTALL");
	if ( convertAll )
		Tool_ConvertData();
}

#include <Gothic/Menu/oMenuSavegame.h>
#include <Gothic/Menu/oMenuChgKeys.h>
#include <Gothic/Game/zSession.h>
void CGameManager::PreRun()
{
	if ( dontStartGame )
		return;

	menu_chgkeys       = new oCMenuChgKeys("MENU_OPT_CONTROLS");
	menu_chgkeys_ext   = new oCMenuChgKeys("MENU_OPT_CONTROLS_EXTKEY");

	menu_save = new oCMenuSavegame("MENU_SAVEGAME_SAVE", oCMenuSavegame::Mode::Save);
	menu_load = new oCMenuSavegame("MENU_SAVEGAME_LOAD", oCMenuSavegame::Mode::Load);

	menu_save->saveMan = savegameManager;
	menu_load->saveMan = savegameManager;

	InitScreen_Menu();

	zoptions->WriteBool("INTERNAL", "gameStartFailed", 0, 0);

	std::string ini = zoptions->ParmValue("ini");
	if (ini.empty())
		ini = "Gothic.ini";

	zoptions->Save(ini);

	while (!exitGame) {
		if (gameSession && gameSession->GetCamera())
			break;
		zCMenu::inGameMenu = 0;
		Menu(0);
		zCMenu::inGameMenu = 1;
	}

	InitScreen_Close();
}

void CGameManager::RenderFrame()
{
	if (backLoop) {
		backLoop->Render();
		backLoop->RenderBlit();
	} else if (gameSession && gameSession->GetCamera()) {
		gameSession->Render();
		gameSession->RenderBlit();
	}
}

bool& chapBool = Value<bool>(0x8C2954);
void CGameManager::Run()
{
	while (!exitGame) {
		if (exitSession) {
			InitScreen_Open();
			vidScreen->SetEnableHandleEvent(1);
			InitScreen_Menu();

			while (!exitGame) {
				if (!exitSession)
					break;
				zCMenu::inGameMenu = 0;
				Menu(0);
				zCMenu::inGameMenu = 1;
			}

			InitScreen_Close();
			exitSession = 0;
		} else {
			sysEvent();
			zCInputCallback::GetInput();
			RenderFrame();

			Cdecl<void()> IntroduceChapter{0x42B220};
			if (chapBool && MenuEnabled())
				IntroduceChapter();
		}
	}
}

void CGameManager::Done()
{
	g2::Log("GameManager", "Exiting game.");

	if (menu_chgkeys)
		delete menu_chgkeys;
	if (menu_chgkeys_ext)
		delete menu_chgkeys_ext;
	if (menu_save)
		delete menu_save;
	if (menu_load)
		delete menu_load;

	menu_chgkeys       = 0;
	menu_chgkeys_ext   = 0;
	menu_save          = 0;
	menu_load          = 0;

	Thiscall<void(CGameManager*)> _g_Done{0x4254E0};
	_g_Done(this);
}

#include <Gothic/Game/zConsole.h>
void g2::InitConsole()
{
	zcon.SetPos(0, 0);
	zcon.SetDim(8191, 1500);
	zcon.SetAutoCompletion(1);

	zcon.Register("ZERR AUTHORS", "Set the author-filter for messages (as characters A-Z)");
	zcon.Register("ZERR SEARCHSPY", "Search for existing zSpy if started later than game.");
	zcon.Register("ZERR LEVEL", "Set the maximum priority-level for messages (from -1 to 10)");
	zcon.Register("ZERR STATUS", "Show error-status (current level, target ...)");
	zcon.Register("ZERR REM", "Include a remark into the error-log.");
	zcon.Register("ZERR ZVIEW", "just for internal tests");

	zcon.Register("SET CLIPPINGFACTOR", "Setting the clipping-factor. Default is 1. Usually check 0.1 ... 2.0");
	zcon.Register("LIST CS", "List running cutscenes.");
	zcon.Register("LIST CS STATES", "List running cutscenes including their current status.");
	zcon.Register("LIST CS PROPS", "List running cutscenes including status and properties.");
	zcon.Register("LIST CS HISTORY", "List all cutscenes in the history-pool.");
	zcon.Register("SHOW CS", "Show information of a cutscene (add cutscene-name including extension)");
	zcon.Register("PLAY CS", "Play a cutscene (add cutscene-name including extension)");
	zcon.Register("PLAY VIDEO", "Playing a videofile");
	zcon.Register("TOGGLE CS LIST", "Toggle list of running cutscenes on screen");
	zcon.Register("TOGGLE CUTSCENES ENABLED", "Enable or disable cutscene-system");
	zcon.Register("HERO EXPORT", "Exports your hero to the given file.");
	zcon.Register("Imports your hero from the given file.", "HERO IMPORT");

	auto zBert_EvalFunc = reinterpret_cast<zCConsole::EvalFunc*>(0x471520);
	auto oBert_EvalFunc = reinterpret_cast<zCConsole::EvalFunc*>(0x430A70);
	zcon.AddEvalFunc( zBert_EvalFunc );
	zcon.AddEvalFunc( oBert_EvalFunc );
}

//#include <aw/utility/filesystem.h>
#include <aw/utility/string/case.h>
void g2::InitGameOptions()
{
	std::string parse;
	if ( zoptions->Parm("PARSE") )
		parse = zoptions->ParmValue( "PARSE" );

	std::string world;
	if ( zoptions->Parm("3D") )
		world = zoptions->ParmValue( "3D" );

	std::string player;
	if ( zoptions->Parm("PLAYER") )
		player = zoptions->ParmValue("PLAYER");

	if ( parse.size() > 0 ) {
		std::string ext = parse.substr(parse.size() - 5);
		// aw::fs::path path(parse);
		// std::string ext = path.extension();
		if ( aw::string::tolower(ext) == ".src" )
			parse.resize(parse.size() - 4);

		if ( parse.size() > 0 ) {
			if ( zgameoptions ) {
				zgameoptions->WriteString("FILES", "Game", parse, 1);
			} else {
				zoptions->WriteString("INTERNAL", "gameScript", parse, 1);
			}
		}
	}
	if ( world.size() > 0 ) {
		if ( zgameoptions ) {
			zgameoptions->WriteString("SETTINGS", "World", world, 1);
		} else {
			zoptions->WriteString("INTERNAL", "gamePath", world, 1);
		}
	}
	if ( player.size() > 0 ) {
		if ( zgameoptions ) {
			zgameoptions->WriteString("SETTINGS", "Player", player, 1);
		} else {
			zoptions->WriteString("INTERNAL", "playerInstanceName", player, 1);
		}
	}

	static int& gLogStatistics = Value<int>(0x8C2B50);
	gLogStatistics = zoptions->ReadBool("INTERNAL", "logStatistics", 0);
}
