#include <Gothic/Game/CGameManager.h>
#include <Gothic/Game/zOptions.h>
#include <Gothic/Game/oSaveGame.h>
#include <Gothic/Audio/zMusic.h>
#include <Gothic/Audio/zSound.h>
#include <Gothic/Menu/zMenu.h>
#include <Gothic/System/System.h>

namespace g2r {
void InitSound()
{
	Log("Engine", "Initializing Sound");
	float masterVol = zoptions->ReadReal("SOUND", "soundVolume", 1.0);
	if ( zsound )
		zsound->SetMasterVolume(masterVol);
}

auto ChangeMusicEnabled = func<zCOptions::ChangeHandler>(0x42D240);
void InitMusic()
{
	Log("Engine", "Initializing Music");
	zoptions->InsertChangeHandler("SOUND", "musicEnabled", ChangeMusicEnabled);
	float musicVol = zoptions->ReadReal("SOUND", "musicVolume", 0.8);
	if ( zmusic )
		zmusic->SetVolume(musicVol);
}

auto DefineMenuScriptFunctions = Cdecl<int()>(0x42C1D0);
void InitMenu()
{
	Log("Engine", "Initializing the menu-system");
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
} // namespace g2r



#include <SDL2/SDL.h>
void CGameManager::Init(void* hwnd)
{
	using namespace g2r;

	sysContextHandle = hwnd;

	Log("Engine", "Initializing GameManager");

	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);

	sysEvent();

	PreGraphicsInit();

	sysEvent();


	savegameManager = new oCSavegameManager();
	savegameManager->Reinit();

	sysEvent();

	g2r::InitSound();
	g2r::InitMusic();
	g2r::InitMenu();
	g2r::InitOptions();


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

	Log("Engine", "GameManager initializing done");
}

namespace zCEngine {
auto Init = Cdecl<int(void*&)>(0x558BE0);
}


#include <Graphics/SplashWindow.h>
#include <Gothic/Game/oObjectFactory.h>
#include <Gothic/Menu/zView.h>
void CGameManager::PreGraphicsInit()
{
	using namespace g2r;

	SplashWindow splash;

	if (!splash.IsRunning())
		Warning("Engine", "Could not create splash window.");

	zCClassDef::EndStartup();

	if ( !zfactory )
		zfactory = new zCObjectFactory;

	zInitOptions();

	bool memProfiler = zoptions->Parm("ZMEMPROFILER");
	//stub in .exe
	//zCMallocGeneric::Init(memProfiler);

	zFILE_VDFS::InitFileSystem();

	sysEvent();

	zresMan = new zCResourceManager;
	bool noResThread = zoptions->Parm("ZNORESTHREAD");
	zresMan->SetThreadingEnabled(!noResThread);

	zBert_StartUp();
	zUlfi_StartUp();

	sysEvent();

	zDieter_StartUp(sysContextHandle);
	zCarsten_StartUp(sysContextHandle);

	bool texConvert = zoptions->Parm("ZTEXCONVERT");
	if ( texConvert ) {
		zSTRING val = zoptions->ParmValue("ZTEXCONVERT");
		zCTexture::ScanConvertTextures(val);
	}

	// zfpuControler->RestoreDefaultControlWord()
	// zfpuControler->SaveCurrentControlWord();
	

	if (zoptions->ReadBool("GAME", "playLogoVideos", 1)) {
		PlayVideo("logo1.bik");
		PlayVideo("logo2.bik");
	}

	sysEvent();

	bool recalc = zoptions->ReadBool("PERFORMANCE", "recalc", 1);
	zoptions->WriteBool("PERFORMANCE", "recalc", 0, 0);

	if ( recalc )
		InitSettings();

	InitScreen_Open();

	if ( zfactory )
		zfactory->Release();

	zfactory = new oCObjectFactory;

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

