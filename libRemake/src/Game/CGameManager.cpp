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
#include <Gothic/Game/zConsole.h>
void CGameManager::PreGraphicsInit()
{
	using namespace g2;

	SplashWindow splash;

	if (!splash.IsRunning())
		Warning("Startup", "Could not create splash window.");

	zCClassDef::EndStartup();

	if ( zfactory )
		zfactory->Release();
	zfactory = new oCObjectFactory;

	Cdecl<void()> zInitOptions{0x4701F0};
	zInitOptions();

	bool memProfiler = zoptions->Parm("ZMEMPROFILER");
	//stub in .exe
	//zCMallocGeneric::Init(memProfiler);

	zFILE_VDFS::InitFileSystem();

	sysEvent();

	zresMan = new zCResourceManager;
	bool noResThread = zoptions->Parm("ZNORESTHREAD");
	zresMan->SetThreadingEnabled(!noResThread);

	Cdecl<void()> zBert_StartUp{0x471230};
	zBert_StartUp();

	zfontman = new g2::FontMan{};

	zcon.SetPos(0, 0);
	zcon.SetDim(8191, 1500);
	zcon.SetAutoCompletion(1);

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


	Cdecl<void()> oBert_StartUp{0x430540};
	oBert_StartUp();

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

			Cdecl<void()> IntroduceChapter{0x6FB4E0};
			// chapBool -- real name unknown
			if (chapBool && MenuEnabled())
				IntroduceChapter();
		}
	}
}

void CGameManager::Done()
{
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
