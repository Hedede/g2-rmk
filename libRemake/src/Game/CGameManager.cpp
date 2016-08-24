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
} // namespace g2r

void CGameManager::Init(void* hwnd)
{
	using namespace g2r;

	sysContextHandle = hwnd;

	Log("Engine", "Initializing GameManager");

	sysEvent();
	GameInit();
	sysEvent();

	bool convertAll = zoptions->Parm("ZCONVERTALL");
	if ( convertAll )
		Tool_ConvertData();

	savegameManager = new oCSavegameManager();
	savegameManager->Reinit();

	sysEvent();

	g2r::InitSound();
	g2r::InitMusic();
	g2r::InitMenu();

	zoptions->ReadInt("PERFORMANCE", "sightValue" , 4);
	zoptions->ReadReal("PERFORMANCE", "modelDetail" , 0.5);
	zoptions->ReadReal("INTERNAL", "texDetailIndex" , 0.6);
	zoptions->ReadInt("GAME", "skyEffects" , 1);
	zoptions->ReadInt("GAME", "bloodDetail" , 2);
	zoptions->ReadReal("VIDEO", "zVidBrightness" , 0.5);
	zoptions->ReadReal("VIDEO", "zVidContrast"   , 0.5);
	zoptions->ReadReal("VIDEO", "zVidGamma"      , 0.5);

	if ( zoptions->Parm("PLAYER") ) {
		zSTRING playerInst = zoptions->ParmValue("PLAYER");
		if ( zgameoptions )
			zgameoptions->WriteString("SETTINGS", "Player", std::string(playerInst), 1);
		else
			zoptions->WriteString("INTERNAL", "playerInstanceName", std::string(playerInst), 1);
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

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
void CGameManager::GameInit()
{
	using namespace g2r;
	SDL_Init(SDL_INIT_VIDEO);

	int screen = 0;
	int posX = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);
	int posY = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);

	auto window = SDL_CreateWindow("Gothic — Loading", posX, posY, 502, 202, SDL_WINDOW_BORDERLESS);

	if (window == nullptr) {
		Warning("Engine", "Could not create splash window: ",  SDL_GetError());
	} else {
		auto surface = SDL_GetWindowSurface(window);

		auto image = IMG_Load("splash.png");
		SDL_BlitSurface(image, NULL, surface, NULL);
		SDL_FreeSurface(image);

		SDL_UpdateWindowSurface(window);
	}

	zCEngine::Init(sysContextHandle);

	SDL_DestroyWindow(window);
/*
	if (zoptions->ReadBool("GAME", "playLogoVideos", 1)) {
		PlayVideo("logo1.bik");
		PlayVideo("logo2.bik");
	}*/

	sysEvent();

	bool recalc = zoptions->ReadBool("PERFORMANCE", "recalc", 1);
	zoptions->WriteBool("PERFORMANCE", "recalc", 0, 0);

	if ( recalc )
		InitSettings();
	/*
	InitScreen_Open();

	if ( zfactory )
		zfactory->Release();

	zfactory = new oCObjectFactory;

	oBert_StartUp();

	vidScreen = screen;
	vidScreen->SetEnableHandleEvent(1);

	sysEvent();
	oCarsten_StartUp();

	zCMenu::EnterCallback = MenuEnterCallback;
	zCMenu::LeaveCallback = MenuLeaveCallback;*/
}

