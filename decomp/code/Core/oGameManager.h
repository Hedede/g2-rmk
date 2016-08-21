//######################################################
//	Game Manager: Kümmert sich darum
//  Game Sessions zu starten zu speichern usw.
//######################################################
struct CGameManager : zCInputCallback {
	CGameManager();
	virtual ~CGameManager();
	int HandleEvent(int key) override;
	bool HandleCancelKey();

	void InsertMenuWorld(zSTRING& backWorld, zSTRING& backDatFile) { }
	void RemoveMenuWorld() { }

	oCGame* GetGame() const
	{
		return this->gameSession;
	}

	int GetPlaytimeSeconds();
	void ShowRealPlayTime();

	int PlayVideoEx(zSTRING fileName, short blendTime, int exit);
	int PlayVideo(zSTRING fileName);

	void ShowIntro() { }
	void ShowExtro() { }

	void ExitGame();
	void ExitSession();

	void GameSessionInit();
	void GameSessionDone();
	void GameSessionReset();

	void ApplySomeSettings();

	int IntroduceChapter();

	bool IsGameRunning();

	void Read_Savegame();
	void Write_Savegame();

	int MenuEnabled(/*zBOOL&*/);
	int Menu(zBOOL inMenu);

protected:
	void InitScreen_Open();
	void InitScreen_Menu();
	void InitScreen_Close();

private:
	zTRnd_AlphaBlendFunc   oldAlphaBlendFunc;
	zTSystemContextHandle  sysContextHandle;

	oCGame*            gameSession;
	oCGame*            backLoop;
	zBOOL              exitGame;
	zBOOL              exitSession;
	zBOOL              gameIdle;
	zBOOL              lastWorldWasGame;
	oCSavegameManager* savegameManager;

	zCArray<zSTRING>	lastDatFileList;
	zCArray<zSTRING>	lastWorldList;

	zSTRING backWorldRunning;
	zSTRING backDatFileRunning;

	zCView*        vidScreen;
	zCView*        initScreen;

	zBOOL          introActive;
	zBOOL          dontStartGame;

	oCBinkPlayer*  videoPlayer;
	zBOOL          videoPlayInGame;

	zCMenu*         menu_chgkeys;
	zCMenu*         menu_chgkeys_ext;
	oCMenuSavegame* menu_save_savegame;
	oCMenuSavegame* menu_load_savegame;

	//wird selten (?) aktualisiert. Mindestens aber beim Speichern und Laden.
	int playTime;
};

bool chapBool;
zSTRING chapName;
zSTRING chapTime;
zSTRING chapTGA;
zSTRING chapWAV;
float chapTime;


HANDLE CGameManager::ShowSplashScreen()
{
	lParam = (LPARAM)LoadBitmapA(hInstApp, (LPCSTR)0xA9);
	hSplashThread = CreateThread(0, 0, SplashThreadProc, 0, 0, &idThread);
	return hSplashThread;
}

void CGameManager::RemoveSplashScreen()
{
	if ( idThread )
		PostThreadMessageA(idThread, 0x12u, 0, 0);
}

void CGameManager::ExitGame()
{
	exitGame = 1;
}

void CGameManager::ExitSession()
{
	exitSession = 1;
	if ( gameSession ) {
		if ( singleStep )
			gameSession->Unpause();
		gameSession->Pause(1);
		oCGame::SetShowPlayerStatus(gameSession, 0);
	}
}

bool CGameManager::IsGameRunning()
{
	return gameSession && gameSession->GetCamera();
}

int CGameManager::GetPlaytimeSeconds()
{
	time(&Time1);

	playTime += fabs(difftime(Time1, Time));

	time(&Time);

	return playTime;
}

void CGameManager::GameDone()
{
	oCarsten_ShutDown();
	zCEngine::Shutdown();
	Delete(zengine);
	zengine = 0;
}


int CGameManager::IntroduceChapter(zSTRING chapter, zSTRING text, zSTRING tga, zSTRING wav, int time)
{
	chapBool = 1;
	chapName = chapter.stdstr;
	chapText = text.stdstr;
	chapTGA  = tga.stdstr;
	chapWAV  = wav.stdstr;
	chapTime = time;
	return 1;
}

CGameManager::CGameManager()
	: zCInputCallback()
{
	gameMan = this;

	gameSession = 0;
	backLoop = 0;
	vidScreen = 0;
	initScreen = 0;
	menu_chgkeys = 0;
	menu_save_savegame = 0;
	menu_load_savegame = 0;
	exitGame = 0;
	exitSession = 0;
	gameIdle = 1;
	lastWorldWasGame = 0;
	introActive = 0;
	dontStartGame = 0;
	savegameManager = 0;

	videoPlayer = new oCBinkPlayer();
	videoPlayInGame = 0;

	playTime = 0;
}

// static
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

int CGameManager::RenderFrame()
{
	if (backLoop) {
		backLoop->Render();
		backLoop->RenderBlit();
	}
	else if (gameSession && gameSession->GetCamera())
	{
		gameSession->Render();
		gameSession->RenderBlit();
	}
}

int RenderFrameCallback()
{
	gameMan->RenderFrame();
}

void CGameManager::Run()
{
	if ( dontStartGame )
		return;

	menu_chgkeys = new oCMenu_ChgKeys("MENU_OPT_CONTROLS");
	menu_chgkeys_ext = new oCMenu_ChgKeys("MENU_OPT_CONTROLS_EXTKEY");
	menu_save_savegame = new oCMenuSavegame("MENU_SAVEGAME_SAVE");
	menu_load_savegame = new oCMenuSavegame("MENU_SAVEGAME_LOAD");

	menu_save_savegame->saveMan = this->savegameManager;
	menu_load_savegame->saveMan = this->savegameManager;

	InitScreen_Menu();

	zoptions->WriteBool(zOPT_SEC_INTERNAL, "gameStartFailed", 0, 0);

	zSTRING ini = zoptions->ParmValue("ini");
	if (ini.IsEmpty())
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
			// chapBool -- real name unknown
			if (chapBool && MenuEnabled())
				IntroduceChapter();
		}
	}

	if (menu_chgkeys)
		delete menu_chgkeys;
	if (menu_chgkeys_ext)
		delete menu_chgkeys_ext;
	if (menu_save_savegame)
		delete menu_save_savegame;
	if (menu_load_savegame)
		delete menu_load_savegame;

	menu_chgkeys = 0;
	menu_chgkeys_ext = 0;
	menu_save_savegame = 0;
	menu_load_savegame = 0;
}

CGameManager::~CGameManager()
{
	if (gameSession)
		zWARNING("B: GMAN: gameSession is existing. Call CGameManager::Done() before!"); // 375

	if ( videoPlayInGame ) {
		videoPlayer->PlayDeinit();
		videoPlayer->CloseVideo();
	}

	
	if (videoPlayer)
		delete videoPlayer;
	if (gameSession)
		delete gameSession;
	if (backLoop)
		delete backLoop;
	if (initScreen)
		delete initScreen;
}

void CGameManager::GameInit()
{
	zerr.onexit = ExitGameFunc;

	lParam = (LPARAM)LoadBitmapA(hInstApp, (LPCSTR)0xA9);
	hSplashThread = CreateThread(0, 0, SplashThreadProc, 0, 0, &idThread);

	if ( !zengine )
		zengine = new zCEngine;

	zengine->Init(sysContextHandle);

	if ( idThread )
		PostThreadMessageA(idThread, 0x12u, 0, 0);

	if (zoptions->ReadBool(zOPT_SEC_GAME, "playLogoVideos", 1)) {
		PlayVideo("logo1.bik");
		PlayVideo("logo2.bik");
	}

	sysEvent();

	bool recalc = zoptions->ReadBool(zOPT_SEC_PERFORMANCE, "recalc", 1);
	zoptions->WriteBool(zOPT_SEC_PERFORMANCE, "recalc", 0, 0);

	if ( recalc )
		CGameManager::InitSettings();

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
	zCMenu::LeaveCallback = MenuLeaveCallback;
}

void CGameManager::InitSettings() // static?
{
	zerr.Separator("");

	oCSystemInfo sysInfo;

	sysInfo.AnalyseNow();

	zerr.Separator("");

	/* Not really necessary (maybe inlined function,
	 * removed by the linker?)
	 * mov     [esp+0C4h+sysInfo.cpuLow],   700.0
	 * mov     [esp+0C4h+sysInfo.cpuHigh],  1200.0
	 * mov     [esp+0C4h+sysInfo.memLow],   256.0
	 * mov     [esp+0C4h+sysInfo.memHigh],  512.0
	 * mov     [esp+0C4h+sysInfo.gmemLow],  16.0
	 * mov     [esp+0C4h+sysInfo.gmemHigh], 64.0
	 */

	sysInfo.ScoreNow();

	zerr.Separator("");

	sysInfo.ResetSettings(-1.0);

	zerr.Separator("");
}


void CGameManager::Tool_ConvertData()
{
	zINFO(1,"B: Converting all data ..."); // 453,

	zCScanDir dirScanner;

	dirScanner.RegisterFileHandler(zCMorphMeshConvertFileHandler(), "zCMorphMesh")
	dirScanner.RegisterFileHandler(zCMorphMeshConvertFileHandler(), "zCMorphMesh")
	dirScanner.RegisterFileHandler(zCProgMeshConvertFileHandler(), "zCProgMeshProto")
	// dirScanner.RegisterFileHandler(zCTextureFileHandler(), "zCTexture")
	// какого-то черта просто вызывается конструктор и ничего дальше не делается

	dirScanner.Scan(zoptions->GetDirString(DIR_DATA));

	zINFO(1,"B: finished") // 469,
}



void CGameManager::Init(HWND& hWndApp)
{
	sysContextHandle = hWndApp;
	zINFO(3,"B: GMAN: Initialize GameManager"); // 476

	if (vidScreen)
		delete vidScreen;
	vidScreen = 0;

	if (gameSession)
		delete gameSession;
	gameSession = 0;


	zINFO(6,"B: GMAN: Initializing the game"); // 487
	sysEvent();
	GameInit();
	sysEvent();

	convertAll = zoptions->Parm("ZCONVERTALL");
	if ( convertAll )
		Tool_ConvertData();

	zoptions->InsertChangeHandler(
	           zOPT_SEC_SOUND,
	           "musicEnabled",
	           ChangeMusicEnabled);

	float masterVol = zoptions->ReadReal(zOPT_SEC_SOUND, ZOPT_SND_SFX_VOL, 1.0);
	if ( zsound )
		zsound->SetMasterVolume(masterVol);

	float musicVol = zoptions->ReadReal(zOPT_SEC_SOUND, ZOPT_SND_MUSIC_VOL, 0.8);
	if ( zmusic )
		zmusic->SetVolume(musicVol);

	savegameManager = new oCSavegameManager();
	savegameManager->Reinit();
	sysEvent();


	zINFO(6,"B: GMAN: Initializing the menu-system"); // 525
	zCMenu::CreateParser();
	DefineMenuScriptFunctions();
	zoptions->ReadBool(zOPT_SEC_INTERNAL, "extendedMenu" , 0);
	zoptions->ReadInt(zOPT_SEC_PERFORMANCE, "sightValue" , 4);
	zoptions->ReadReal(zOPT_SEC_PERFORMANCE, "modelDetail" , 0.5);
	zoptions->ReadReal(zOPT_SEC_INTERNAL, "texDetailIndex" , 0.60000002);
	zoptions->ReadInt(zOPT_SEC_GAME, "skyEffects" , 1);
	zoptions->ReadInt(zOPT_SEC_GAME, "bloodDetail" , 2);
	zoptions->ReadReal(zOPT_SEC_VIDEO, "zVidBrightness" , 0.5);
	zoptions->ReadReal(zOPT_SEC_VIDEO, "zVidContrast"   , 0.5);
	zoptions->ReadReal(zOPT_SEC_VIDEO, "zVidGamma"      , 0.5);


	zINFO(6,"B: GMAN: Analysing commandline-parameters"); // 550

	player = zoptions->Parm("PLAYER");
	if ( player )
	{
		zSTRING playerInst = zoptions->ParmValue("PLAYER");
		if ( zgameoptions )
		{
			zgameoptions->WriteString(zOPT_SEC_SETTINGS, "Player", playerInst, 1);
		}
		else
		{
			zoptions->WriteString(zOPT_SEC_INTERNAL, "playerInstanceName", playerInst, 1);
		}
	}

	sysEvent();
	SetEnableHandleEvent(true);

	GameSessionInit();

	sysEvent();

	zCMenu::Startup();

	zINFO(3,""); // 572
}

void CGameManager::Done()
{
	if ( !dontStartGame ) {
		zINFO(3,"B: GOTHIC: Exiting game ..."); // 628

		zCCacheBase::S_ClearCaches();
		sysKillWindowsKeys(0);

		bool fastExit = zCOption::Parm(zoptions, "ZNOFASTEXIT") == 0;
		if ( fastExit ) {
			_OSVERSIONINFOA VersionInformation;
			if ( GetVersionExA(&VersionInformation) != 1 ||
					VersionInformation.dwPlatformId != 1 )
			{
				zDieter_ShutDown_Fast();
			}
			else
			{
				zDieter_ShutDown(0);
				zCarsten_ShutDown();
			}

			exit(0);
		}

		GameSessionDone();

		oCarsten_ShutDown();
		zCEngine::Shutdown();

		if ( zengine )
			delete zengine;
		zengine = 0;

		if ( savegameManager )
			delete savegameManager;
		savegameManager = 0;
	}
}

void CGameManager::InitScreen_Open()
{
	zINFO(4,"B: GMAN: Open InitScreen"); // 811

	if (initScreen)
		InitScreen_Close();

	initScreen = new zCView(0, 0, 0x2000, 0x2000, 2);
	zoptions->ChangeDir(13);
	screen->InsertItem(initScreen, 0);

	initScreen->InsertBack("startscreen.tga");

	zrenderer->vtab->BeginFrame();
	zCView::Render(screen);
	zrenderer->vtab->EndFrame();

	zrenderer->vtab->Vid_Blit(1, 0, 0);
}

void CGameManager::InitScreen_Menu()
{
	zINFO(4,"B: GMAN: MenuScreen"); // 836

	zrenderer->BeginFrame();
	screen->Render();
	zrenderer->EndFrame();
	zrenderer->Vid_Blit(1, 0, 0);
}

void CGameManager::InitScreen_Close()
{
	zINFO(4,"B: GMAN: Close InitScreen"); // 849

	if (initScreen) {
		delete initScreen;
		initScreen = 0;
	}
}

void CGameManager::PreMenu()
{
	zoptions->SetFlag(zOPT_SEC_VIDEO, ZOPT_VID_RES, ENTRY_DONT_SAVE);

	auto resX = zoptions->ReadInt(zOPT_SEC_VIDEO, "zVidResFullscreenX", 800);
	auto resY = zoptions->ReadInt(zOPT_SEC_VIDEO, "zVidResFullscreenY", 600);
	auto bpp = zoptions->ReadInt(zOPT_SEC_VIDEO, "zVidResFullscreenBPP", 16);

	int modeId = -1;
	int secureId = -1;
	int modeInfo[3];
	for (int i = 0; i < zrenderer->Vid_GetNumModes(); ++i) {
		zrenderer->Vid_GetModeInfo(&modeInfo, i);
		if (!VidIsResolutionValid(modeinfo[0], modeinfo[1], modeInfo[2]))
			continue;

		if ( modeInfo[0] == resX && modeInfo[1] == resY && modeInfo[3] == bpp) {
			modeId = i;
			zINFO(3,"B: VID: Videomode found");// 1112,
		}

		if ( modeInfo[0] == 800 && modeInfo[1] == 600 && modeInfo[3] == 16) {
			secureId = i;
			zINFO(3,"B: VID: Secure videomode found: 800x600x16"); // 1116
		}
	}

	if ( modeId < 0 ) {
		modeId = secureId;
		zoptions->WriteInt(zOPT_SEC_VIDEO, "zVidResFullscreenX", 800, 0);
		zoptions->WriteInt(zOPT_SEC_VIDEO, "zVidResFullscreenY", 600, 0);
		zoptions->WriteInt(zOPT_SEC_VIDEO, "zVidResFullscreenBPP", 16, 0);
	}

	zoptions->WriteInt(zOPT_SEC_INTERNAL, ZOPT_VID_RES, modeId, 0);
	auto texSize = zoptions->ReadInt(zOPT_SEC_VIDEO, "zTexMaxSize", 256);
	float detailIndex = 0;

	if ( texSize > 512 )
		detailIndex = 1.0;
	if ( texSize > 256 )
		detailIndex = 0.8;
	if ( texSize > 128 )
		detailIndex = 0.6;
	if ( texSize > 64 )
		detailIndex = 0.4;
	if ( texSize > 32 )
		detailIndex = 0.2;
	else
		detailIndex = 0.1;

	zoptions->WriteReal(zOPT_SEC_INTERNAL, "texDetailIndex", detailIndex, 0);

	auto mouseEnabled = zoptions->ReadBool(zOPT_SEC_GAME, "enableMouse", 1);
	auto mouseSens = zoptions->ReadReal(zOPT_SEC_GAME, "mouseSensitivity", 0.5);

	zinput->SetDeviceEnabled(2, mouseEnabled);
	zinput->SetMouseSensitivity(mouseSens * 0.5 + 0.3, mouseSens * 0.5 + 0.3);

	auto joystickEnabled = zoptions->ReadBool(zOPT_SEC_GAME, "enableJoystick", 0);
	zinput->SetDeviceEnabled(3, joystickEnabled);
}

void CGameManager::PostMenu(zSTRING action)
{
	ApplySomeSettings();
	if ( gameSession )
		gameSession->UpdateScreenResolution(v3);
	zinput->ProcessInputEvents();
	zinput->ClearKeyBuffer();
	zinput->ResetRepeatKey(1);
}

// WHAT the crap? why zBOOL IS NEEDED HERE?
// THERE WAS NO ARGUMENT IN G1 VERSION OF THIS FUNCTION!
// also, in G1 it is cleaner
int CGameManager::MenuEnabled(/* zBOOL& out */)
{
	if ( !oCNpc::player || !gameSession || !gameSession->GetCamera())
		return 0;

	if (gameSession->GetWorld()->csPlayer->GetPlayingGlobalCutscene())
		return 0;

	auto bodyState = oCNpc::player->GetBodyState();
	auto talkPartner = oCNpc::player->GetTalkingWith();

	auto zs_talk = zparser.GetIndex("ZS_TALK");
	auto infoman = oCInformationManager::GetInformationManager();

	if ( bodyState == BS_FALL      ||
	     bodyState == BS_INVENTORY ||
	     bodyState == BS_PICKPOCKET)
		return 0;

	if (!infoman.HasFinished())
		if (talkPartner && talkPartner->states->GetState() == zs_talk)
			return 0;

	if (!oCNpc::player || oCNpc::game_mode || oCNpc::player->inventory.IsOpen())
		return 0;

	if (bodyState == BS_MOBINTERACT           ||
	    bodyState == BS_MOBINTERACT_INTERRUPT ||
	    bodyState == BS_SIT                   ||
	    bodyState == BS_LIE                   ||
	    bodyState == BS_ITEMINTERACT          ||
	    bodyState == BS_TAKEITEM              ||
	    bodyState == BS_DROPITEM              ||
	    bodyState == BS_CASTING)
		return 0;

	if (oCNpc::player->HasBodyStateModifier(BS_MOD_CONTROLLED) ||
	    oCNpc::player->HasBodyStateModifier(0x2000) ||
	    oCNpc::player->HasBodyStateModifier(BS_MOD_TRANSFORMED)
	    oCNpc::player->human_ai->IsDead())
		return 0;

	return 1;
}

void CGameManager::GameSessionReset()
{
	if ( !gameSession )
		zFATAL("B: GameMan: Call GameSessionInit before GameSessionReset() "); // 1039

	savegameManager->ClearCurrent();
	gameSession->SetGameInfo(0);
}

void CGameManager::GameSessionInit()
{
	if ( gameSession )
		zFATAL("B: GameMan: Call GameSessionInit once, not twice!"); // 1047,

	gameSession = new oCGame();
	ogame = gameSession;

	gameSession->Init();
	gameSession->savegameManager = savegameManager;
	gameSession->SetGameInfo(gameSession, 0);
}

void CGameManager::GameSessionDone()
{
	if ( !gameSession )
		zFATAL("B: GameMan: Call GameSessionInit before GameSessionDone"); // 1057,

	gameSession->Done();
	Delete(gameSession);
	ogame = 0;
}

void CGameManager::ApplySomeSettings()
{
	zINFO(3,"B: OPT: Applying settings ..."); //1178,

	auto readTemp_old = zoptions->readTemp;
	zoptions->readTemp = 0;

	auto extendedMenu   = zoptions->ReadBool(zOPT_SEC_INTERNAL, "extendedMenu", 0);
	auto texDetailIndex = zoptions->ReadReal(zOPT_SEC_INTERNAL, "texDetailIndex", 1.0);
	int sightValue   = zoptions->ReadInt(zOPT_SEC_PERFORMANCE, "sightValue", 4);
	auto modelDetail = zoptions->ReadReal(zOPT_SEC_PERFORMANCE, "modelDetail", 0.5);
	int skyEffects   = zoptions->ReadInt(zOPT_SEC_GAME, "skyEffects", 1);
	int bloodDetail  = zoptions->ReadInt(zOPT_SEC_GAME, "bloodDetail", 2);

	auto brightness  = zoptions->ReadReal(zOPT_SEC_VIDEO, "zVidBrightness", 0.5);
	auto contrast    = zoptions->ReadReal(zOPT_SEC_VIDEO, "zVidContrast", 0.5);
	auto gamma       = zoptions->ReadReal(zOPT_SEC_VIDEO, "zVidGamma", 0.5);

	auto sfxVol   = zoptions->ReadReal(zOPT_SEC_SOUND, ZOPT_SND_SFX_VOL, 1.0);
	auto musicVol = zoptions->ReadReal(zOPT_SEC_SOUND, ZOPT_SND_MUSIC_VOL, 0.8);
	bool camLookaroundInverse = zoptions->ReadBool(zOPT_SEC_GAME, "camLookaroundInverse", 0);
	bool enableMouse = zoptions->ReadBool(zOPT_SEC_GAME, "enableMouse", 1);
	auto mouseSensitivity = zoptions->ReadReal(zOPT_SEC_GAME, "mouseSensitivity", 0.5);
	auto vobFarClipZScale = zoptions->ReadReal("ENGINE", "zVobFarClipZScale", 4.0);
	bool waterAni    = zoptions->ReadBool("ENGINE", "zWaterAniEnabled", 1);
	bool envMapping  = zoptions->ReadBool("ENGINE", "zEnvMappingEnabled", 1);
	bool clipFade    = zoptions->ReadBool("ENGINE", "zFarClipAlphaFade", 1);
	bool windEnabled = zoptions->ReadBool("ENGINE", "zWindEnabled", 1);
	bool ambientVobs = zoptions->ReadBool("ENGINE", "zAmbientVobsEnabled", 1);
	auto ambientPFXEnabled = zoptions->ReadBool("ENGINE", "zAmbientPFXEnabled", 1);
	bool radialFog   = zoptions->ReadBool("GAME", "zFogRadial", 1);
	auto cloudShadow = zoptions->ReadReal("ENGINE", "zCloudShadowScale", 0.0);
	float speechReverb = zoptions->ReadReal("GAME", "useSpeechReverbLevel", 1.0);
	auto meleeFocus    = zoptions->ReadInt("GAME", "highlightMeleeFocus", 0);
	auto interFocus    = zoptions->ReadBool("GAME", "highlightInteractFocus", 0);
	bool g1Controls    = zoptions->ReadBool("GAME", "useGothic1Controls", 1);

	zCAICamera::lookAroundSgn = 2 * (camLookaroundInverse == 0) - 1;

	zINFO(3,"B: OPT: Lookaround inverse = "_s + camLookaroundInverse); // 1221,

	int soundProviderIndex = zoptions->ReadInt(zOPT_SEC_INTERNAL, "soundProviderIndex", 0);
	int soundSpeakerIndex = zoptions->ReadInt(zOPT_SEC_INTERNAL, "soundSpeakerIndex", 0);

	zsound->SetProvider(soundProviderIndex);
	zsound->SetSpeakerType(soundSpeakerIndex);

	zINFO(3,"B: OPT: Music-Volume: Value="_s + musicVol); // 1233,
	zINFO(3,"B: OPT: Sound-Volume: Value="_s + sfxVol); // 1234,

	zinput->SetDeviceEnabled(2, enableMouse);

	float sens = mouseSensitivity * 0.5 + 0.3;
	zinput->SetMouseSensitivity(zinput, sens, sens);

	zINFO(3,"B: OPT: Mouse-Enabled = "_s + enableMouse); // 1240,
	zINFO(3,"B: OPT: Mouse-Sensitivity = "_s + sens); // 1241,
	if ( gameSession ) {
		auto sight = 4 * (5 * sightValue + 5);
		zClamp(sight,20,300);
		float sightFactor = sight * 0.01;

		auto world = gameSession->GetWorld();
		world->GetActiveSkyControler()->SetFarZScalability(sightFactor);

		zINFO(3,"B: OPT: Level-Sight: Value="_s + sightValue + ", resulting factor="_s + sightFactor); // 1251,

		world->vobFarClipZScalability = vobFarClipZScale * 0.22142857 + 0.4;
		zINFO(3,"B: OPT: Vob-Sight: Value=" + vobFarClipZScale); // 1254,

		zCParticleFX::s_bAmbientPFXEnabled = ambientPFXEnabled;
		zCVob::s_enableAnimations     = windEnabled;
		zCWorld::s_bEnvMappingEnabled = envMapping;
		zCWorld::s_bWaveAnisEnabled   = waterAni;
		zCWorld::s_bFadeOutFarVerts   = clipFade;
		oCGame::s_bUseOldControls     = g1Controls;

		if ( zrenderer )
			zrenderer->SetRadialFog(radialFog);

		if ( zCSkyControler::s_activeSkyControler ) {
			float sf = cloudShadow == 0.0 ? 0.0 : 1.0;
			zCSkyControler::s_activeSkyControler->SetCloudShadowScale(sf);
		}

		oCGame::s_fUseSpeechReverb = speechReverb;
		oCNpcFocus::s_iHightlightMeleeFocus = meleeFocus;
		oCNpcFocus::s_bHighlightInteractFocus = interFocus;
		zCSkyControler::s_skyEffectsEnabled = skyEffects == 1;

		zINFO(3,"B: OPT: Sky: Value="_s + skyEffects); // 1271
		auto texSize = zoptions->ReadInt(zOPT_SEC_VIDEO, "zTexMaxSize", -1);

		int texSizeI = 0x4000;
		zClamp(texDetailIndex, 0.0, 1.0);
		if (texDetailIndex < 0.15) {
			texSizeI = 32;
		} else if (texDetailIndex < 0.35) {
			texSizeI = 64;
		} else if (texDetailIndex < 0.55) {
			texSizeI = 128;
		} else if (texDetailIndex < 0.75) {
			texSizeI = 256;
		} else if (texDetailIndex < 0.95) {
			texSizeI = 512;
		}

		if ( texSizeI != texSize ) {
			zCTexture::RefreshTexMaxSize(texSizeI);
			if ( zresMan )
				zresMan->PurgeCaches(&zCTexture::classDef);

			zINFO(3, "B: OPT: TextureDetails: Value="_s + texDetailIndex + ", resulting size = "_s + texSizeI); // 1288
		}

		zClamp(modelDetail, 0.0, 1.0);
		zCProgMeshProto::SetLODParamStrengthModifier(modelDetail);
		zINFO(3,"B: OPT: Model-Details: Value="_s + modelDetail); // 1294

		zClamp(bloodDetail, 0, 3);
		oCNpc::modeBlood = bloodDetail;

		zINFO(3,"B: OPT: Blood-Details: Value="_s + bloodDetail); // 1302,

		bool subTitles = zoptions->ReadBool(zOPT_SEC_GAME, "subTitles", 0);

		if ( zgameoptions && zgameoptions->ReadBool(zOPT_SEC_OPTIONS, "force_Subtitles", 0) )
			subTitles = 1;

		oCNpc::isEnabledTalkBox = subTitles;
		oCNpc::isEnabledTalkBoxPlayer = zoptions->ReadBool(zOPT_SEC_GAME, "subTitlesPlayer", 1);
		oCNpc::isEnabledTalkBoxAmbient = zoptions->ReadBool(zOPT_SEC_GAME, "subTitlesAmbient", 1);
		oCNpc::isEnabledTalkBoxNoise = zoptions->ReadBool(zOPT_SEC_GAME, "subTitlesNoise", 0);

		oCInformationManager::GetInformationManager().UpdateViewSettings();
		if ( ogame )
			ogame->UpdateViewSettings();
	}

	if ( zrenderer )
		zrenderer->Vid_SetGammaCorrection(gamma, contrast, brightness);

	zoptions->readTemp = readTemp_old;

	auto ini = zoptions->ParmValue("ini");
	if (!ini)
		ini = "Gothic.ini";

	zoptions->Save(ini);

	zINFO(3,""); 1337,
}

void CGameManager::Menu(int inMenu)
{
	zINFO(3,"B: GMAN: Entering Menu-Section"); //1350,

	zinput->ResetRepeatKey(1);
	gameSession->SetShowPlayerStatus(0);

	zoptions->WriteString("internal", "menuAction", "RESUME_GAME", 0);

	if (exitGame)
		return;

	std::string::string(&msg.data, "NOMENU", &v42);
	msg.vtable = &zSTRING::`vftable';
	LOBYTE(v49) = 3;
	NOMENU = zoptions->Parm(&msg);
	LOBYTE(v49) = 1;
	msg.vtable = &zSTRING::`vftable';
	std::string::_Tidy(&msg.data, 1);
	CGameManager::PreMenu();
	gameSession = this->gameSession;
	if ( gameSession && gameSession->GetCamera(gameSession) ) {
		if ( NOMENU ) {
			exitGame = 1;
			return;
		}

		zINFO(4,"B: GMAN: Menu started (ingame)") // 1396,

		if ( gameSession )
			gameSession->Pause(exitSession);

		auto inGameMenu = zCMenu::inGameMenu;
		if ( !inMenu )
			zCMenu::inGameMenu = 0;

		auto gameMenu = zCMenu::Create("MENU_MAIN");

		gameMenu->Run();
		gameMenu->Release();

		zCMenu::inGameMenu = inGameMenu;

		if ( gameSession )
			gameSession->Unpause();

		zINFO("B: GMAN: Menu finished"); // 1410,
	} else {
		if ( NOMENU ) {
			zoptions->WriteString("internal", "menuAction", "NEW_GAME", 0);
		} else {
			zINFO(4,"B: GMAN: Menu started (outgame)"); //1375,

			auto mainmenu = zCMenu::Create("MENU_MAIN");

			mainmenu->Run();
			mainmenu->Release();

			zINFO(4,"B: GMAN: Menu finished"); // 1381,
		}
	}

	if ( zrenderer ) {
		screen->DrawItems();
		zrenderer->Vid_Blit(0, 0, 0);
	}

	auto menuAction = zoptions->ReadString("internal", "menuAction", 0);

	zINFO("B: MENU: " + menuAction); // 1430

	if ( menuAction == "LEAVE_GAME") {
		zINFO(5,"B: GMAN: Menu-Selection \"exit\"");// 1437,

		zoptions->WriteBool(zOPT_SEC_INTERNAL, "gameAbnormalExit", 0, 0);
		goto LABEL_85;
	} else if (menuAction == "NEW_GAME") {
		zSTRING world;
		if ( zgameoptions )
			world = zgameoptions->ReadString(zOPT_SEC_SETTINGS, "World", 0);
		else
			world = zoptions->ReadString(zOPT_SEC_INTERNAL, "gamePath", 0);

		if (!world)
			world = "NewWorld\\NewWorld.zen";

		zINFO(4,"B: GMAN: Menu-Selection NEW_GAME "); // 1463,

		GameSessionReset();
		exitSession = 0;

		zINFO(1,"B: GMAN: Loading the World ..."); // 1470,

		v40 = SAVEGAME_SLOT_NEW;
		gameSession->LoadGame(SAVEGAME_SLOT_NEW, world);

		if ( zmusic )
			zmusic->Stop();

		zINFO(1,"B: GMAN: Completed loading the world ..."); // 1476,

		playTime = 0;
	} else if (menuAction == "SAVEGAME_LOAD") {
		Read_Savegame(menu_load_savegame->GetSelectedSlot());
	} else if (menuAction == "SAVEGAME_SAVE") {
		Write_Savegame(menu_save_savegame->GetSelectedSlot());
	} else if (menuAction == "RESUME_GAME") {
		//
	} else {
		zFATAL("C: oGameManager.cpp(CGameManager::Menu()): Menu Selection not known :" + menuAction); // 1516,
	}

	auto ini = zoptions->ParmValue("ini");
	if (!ini)
		ini = "Gothic.ini";

	zoptions->Save(ini);

	introActive = 0;

	ApplySomeSettings();

	if ( gameSession )
		gameSession->UpdateScreenResolution();

	zinput->ProcessInputEvents();
	zinput->ClearKeyBuffer();
	zinput->ResetRepeatKey(1);

	if ( !exitSession )
		gameSession->SetShowPlayerStatus(1);

	zinput->ProcessInputEvents();
	zinput->ClearKeyBuffer();
	zinput->ResetRepeatKey(1);

	zINFO(3,"B: GMAN: Leaving Menu-Section"); // 1537,
}

void CGameManager::Read_Savegame(int slot)
{
	if ( slot >= 0 ) {
		auto sav = savegameManager->GetSavegame(slot);
		if ( sav && !sav->saveIncompatible ) {
			if ( sav->DoesSavegameExist() ) {
				zINFO(4, "B: GMAN: Loading game from slot "_s + slot); //1548

				exitSession = 0;

				GameSessionReset();

				gameSession->LoadSavegame(sav->slot, 1);

				if ( zmusic )
					zmusic->Stop();

				playTime = sav->playTimeSeconds;

				time(&Time);
			}
		}
	}
}

void CGameManager::Write_Savegame(int slot)
{
	if ( slot >= 0 ) {
		auto sav = savegameManager->GetSavegame(slot);
		auto name = sav->GetName();

		if ( name.IsEmpty() ) {
			name = sav->GetWorldName() + "... ";
			sav->title = name;
		}

		zINFO(4,"B: GMAN: Saving game to slot "_s + slot); // 1574, _bert\\oGameManager.cpp

		playTimeSeconds = GetPlaytimeSeconds();

		int day, hour, min;
		ogame->GetTime(day, hour, min);

		sav->worldName = ogame->GetWorld()->GetWorldName();
		sav->timeDay = day;
		sav->timeHour = hour;
		sav->timeMin = min;

		auto texconvert = zrenderer->CreateTextureConvert();

		gameSession->Render();

		zrenderer->Vid_GetFrontBufferCopy(texconvert);

		gameSession->WriteSavegame(sav->slot, 1);
		sav->UpdateThumbPic(texconvert);

		Delete(texconvert);

		savegameManager->SetAndWriteSavegame(slot, sav);
	}
}

bool CGameManager::HandleCancelKey(int key)
{
	if ( !gameSession )
		return 0;
	if ( !gameSession->GetCamera() )
		return 0;
	if ( !gameSession->GetWorld() )
		return 0;
	if ( !oCNpc::player )
		return 0;

	auto world = gameSession->GetWorld();
	auto csPlayer = world->csPlayer;

	if ( !csPlayer )
		return 0;

	if ( csPlayer->GetPlayingGlobalCutscene() ) {
		auto globCs = csPlayer->GetPlayingGlobalCutscene(csPlayer);
		if ( globCs->TryToCancel() ) {
			zINFO(4,"B: CS: User canceled global cutscene"); //1608,
		} else {
			zINFO(4,"B: CS: User tried to cancel global cutscene: Not allowed at this moment!"); // 1610,
		return 1;
	}

	if ( !oCNpc::player || !oCNpc::player->GetTalkingWith() )
		return 0;

	zINFO(4,"B: CS: User tried to cancel OutputUnits ..."); // 1619,

	int numOu1 = csPlayer->StopAllOutputUnits(oCNpc::player);
	int numOu2 = csPlayer->StopAllOutputUnits(oCNpc::player->GetTalkingWith());
	int numOu = numOu1 + numOu2;

	oCNpc::player::StopAllVoices();
	oCNpc::player->GetTalkingWith()->StopAllVoices();
	oCNpc::player->StopFaceAni(NPC_TALK_ANI);
	oCNpc::player->GetTalkingWith()->StopFaceAni(NPC_TALK_ANI);

	zINFO(4,"");// 1631,

	return numOu > 0;
}

int CGameManager::HandleEvent(int key)
{
	if ( !gameSession )
		return 0;

	int menu = 1;
	int ret = 0;

	switch (key) {
	case KEY_F5:
		if ( oCGame::s_bUseQuickSave && zinput->KeyToggled(KEY_F5) )
			if ( gameSession && gameSession->GetCamera() ) {
				if (!gameSession->game_testmode != zinput->KeyPressed(KEY_LCONTROL) ) {
					if ( MenuEnabled(menu) ) {
						gameMan->Write_Savegame(SAVEGAME_SLOT_QUICK);
						ret = 1;
					}
				}
			}
		}
		return 1;

	case KEY_F9:
		if (oCGame::s_bUseQuickSave && zinput->KeyToggled(KEY_F9)) {
			if (!gameSession || (!gameSession->game_testmode != zinput->KeyPressed(zinput, KEY_LCONTROL))) {
				gameMan->Read_Savegame(SAVEGAME_SLOT_QUICK);
				ret = 1;
			}
		}
		return ret;

	case KEY_ESCAPE:
		if (!zinput->KeyPressed(KEY_LSHIFT)) {
			if ( MenuEnabled(menu) ) {
				Menu(menu);
				return 1;
			}

			ret = HandleCancelKey(key);
			if ( ret ) {
				zinput->ClearKeyBuffer();
				zinput->ResetRepeatKey(1);
			}
			return ret;
		}

		Menu(0);
		return 1;

	case MOUSE_BUTTONRIGHT:
		ret = CGameManager::HandleCancelKey(this, key);
		if ( ret ) {
			zinput->ClearKeyBuffer();
			zinput->ResetRepeatKey(1);
		}
		return ret;

	default:
		if (key > KEY_F9)
			break;
		return 0;
	}

	if ( !oCGame::s_bUsePotionKeys )
		return ret;
	if ( !zinput->KeyToggled(key) )
		return ret;

	if ( !gameSession || !gameSession->GetCamera())
		return ret;

	if (gameSession->game_testmode)
		return ret;
	if ( !oCNpc::player )
		return ret;

	auto lame = zinput->GetFirstBindedLogicalKey(key) - GAMEKEY_LAME_POTION;
	int func = -1;
	if (lame == 0) {
		func = zparser.GetIndex("PLAYER_HOTKEY_LAME_HEAL");
	} else if ( lame == 1 ) {
		func = zparser.GetIndex("PLAYER_HOTKEY_LAME_POTION");
	}

	if ( func <= 0 )
		return ret;

	auto msg = new oCMsgManipulate(EV_CALLSCRIPT, func);
	oCNpc::player->GetEM()->OnMessage(msg, oCNpc::player);
	return 1;
}

int CGameManager::PlayVideo(zSTRING fileName)
{
	if ( videoPlayer ) {
		videoPlayInGame = 0;
		bool musicEnabled = zoptions->ReadBool(zOPT_SEC_SOUND, "musicEnabled", 1);
		if ( zsound )
			zsound->StopAllSounds();

		zCZoneMusic::SetAutochange(0);
		zCMusicSystem::DisableMusicSystem(1);

		if ( gameSession )
			gameSession->Pause(exitSession);

		auto dirName = zoptions->GetDirString(DIR_VIDEO);

		videoPlayer->OpenVideo(dirName + fileName);

		videoPlayer->SetFullscreen(1, "videoback.tga");
		videoPlayer->Play(0);
		videoPlayer->CloseVideo();

		if ( gameSession && !exitSession )
			gameSession->Unpause();

		zCZoneMusic::SetAutochange(musicEnabled);
		zCMusicSystem::DisableMusicSystem(!musicEnabled);

		return true;
	}

	return false;
}

int CGameManager::PlayVideoEx(zSTRING fileName, short blendTime, int exit)
{
	if ( videoPlayer ) {
		videoPlayInGame = 0;
		bool musicEnabled = zoptions->ReadBool(zOPT_SEC_SOUND, "musicEnabled", 1);

		if ( zsound )
			zsound->StopAllSounds();

		zCZoneMusic::SetAutochange(0);
		zCMusicSystem::DisableMusicSystem(1);

		if ( gameSession )
			gameSession->Pause(exitSession);

		auto dirName = zoptions->GetDirString(DIR_VIDEO);

		videoPlayer->OpenVideo(dirName + fileName);

		videoPlayer->SetFullscreen(1, "videoback.tga");
		videoPlayer->Play(0);
		if ( blendTime && !exit && oCNpc::player ) {
			auto vfx = oCVisualFX::CreateAndPlay("BLACK_SCREEN", oCNpc::player, 0, 1, 0.0, 0, 0);
			Release(vfx);
		}

		videoPlayer->CloseVideo();

		if ( exit ) {
			ExitSession();
		} else {
			if ( gameSession )
				gameSession->Unpause();
		}

		zCZoneMusic::SetAutochange(musicEnabled);
		zCMusicSystem::DisableMusicSystem(!musicEnabled);
	} else {
		if (!exit)
			return 0;

		ExitSession();
	}

	return 1;
}

void CGameManager::ShowRealPlayTime()
{
	int playTimeSec = GetPlaytimeSeconds();
	int playTimeHours = playTimeSec / 3600;
	int playTimeMin   = playTimeSec / 60 - 60 * playTimeHours;

	if ( ogame->array_view[0] ) {
		zSTRING ptstr;
		ptstr += playTimeHours + ":"_s;
		if ( playTimeMin < 10 )
			ptstr += "0";
		ptstr += playTimeMin + " played"_s;

		ogame->array_view[0]->PrintTimed(100, 300, ptstr, 3000.0, 0);
	}
}

int CGameManager::IntroduceChapter()
{
	if ( !chapBool )
		return 0;

	chapBool = 0;

	if ( zsound ) {
		if (!chapWAV.IsEmpty()) {
			auto sfx = zsound->LoadSavegame(chapWAV);
			zsound->PlaySound(sfx, 0);
			Release(sfx);
		}
	}

	auto chapView = new zCView(0, 0, 0x2000, 0x2000, 2);
	screen->InsertItem(chapView, 0);
	chapView->InsertBack(chapTGA);

	int x = screen->anx(800);
	int y = screen->any(600);

	chapView->SetPos((0x2000 - x) / 2, (0x2000 - y) / 2);
	chapView->SetSize(x, y);

	auto chapText1 = new zCView(0, 0, 0x2000, 0x2000, 2);
	chapView->InsertItem(chapText1, 0);
	chapText1->SetPos(0, 700);
	chapText1->SetSize(0x2000, 600);

	zSTRING font = "font_old_20_white.tga";
	auto sym = zparser.GetSymbol("TEXT_FONT_20");

	if (sym)
		sym->GetValue(font, 0);

	chapText1->SetFont(font);
	chapText1->PrintCXY(chapName);

	auto chapText2 = new zCView(0, 0, 0x2000, 0x2000, 2);
	chapView->InsertItem(chapText2, 0);

	chapText2->SetPos(0, 7100);
	chapText2->SetSize(0x2000, 600);

	zSTRING font_small = "font_old_10_white.tga";
	auto sym = zparser.GetSymbol("TEXT_FONT_10");

	if (sym)
		sym->GetValue(font_small, 0);

	chapText2->SetFont(font_small);
	chapText2->PrintCXY(chapText);

	chapView->Blit();
	chapText1->Blit();
	chapText2->Blit();
	zrenderer->Vid_Blit(1, 0, 0);

	zCTimer timer;
	float time = 0.0;
	do {
		zinput->ProcessInputEvents();
		time += timer.frameTime;

		timer.ResetTimer();

		if ( chapTime < time )
			break;
		if ( zinput->KeyPressed(KEY_ESCAPE) )
			break;
		if ( zinput->GetMouseButtonPressedRight() )
			break;
		zinput->GetState(0xEu);
	} while ( chapTime <= 0.0 );
	// ^ recheck, doesn't make sense

	chapView->RemoveItem(chapText2);
	chapView->RemoveItem(chapText1);
	screen->RemoveItem(chapView);

	delete chapView;
	delete chapText1;
	delete chapText2;

	zinput->ClearKeyBuffer();
	zinput->ResetRepeatKey(1);

	return 1;
}

