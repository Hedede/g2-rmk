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

void CGameManager::Init(HWND& hWndApp)
{
	sysContextHandle = hWndApp;
	zERROR::Message("B: GMAN: Initialize GameManager");

	if (vidScreen)
		delete vidScreen;
	vidScreen = 0;

	if (gameSession)
		delete gameSession;
	gameSession = 0;


	zERROR::Message("B: GMAN: Initializing the game");
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


	zERROR::Message("B: GMAN: Initializing the menu-system");
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


	zERROR::Message("B: GMAN: Analysing commandline-parameters");

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
	if ( gameSession )
	{
		zERROR::Fatal("B: GameMan: Call GameSessionInit once, not twice!");
	}

	gameSession = new oCGame();
	ogame = ocgame;
	gameSession->Init();
	gameSession->savegameManager = this->savegameManager;
	gameSession->SetGameInfo(0);
	sysEvent();
	zCMenu::Startup();

	zERROR::Message("");
}

void CGameManager::InitScreen_Menu()
{
	zERROR::Message("B: GMAN: MenuScreen");

	zrenderer->BeginFrame();
	screen->Render();
	zrenderer->EndFrame();
	zrenderer->Vid_Blit(1, 0, 0);
}

void CGameManager::InitScreen_Close()
{
	zINFO(4,"B: GMAN: Close InitScreen");

	if (initScreen) {
		delete initScreen;
		initScreen = 0;
	}
}

void CGameManager::InitScreen_Open()
{
	zINFO("B: GMAN: Open InitScreen");

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

// static
void CGameManager::RenderFrame()
{
	if (backLoop) {
		backLoop->Render();
		backLoop->RenderBlit();
	} else {
		if (gameSession && gameSession->GetCamera())
		{
			gameSession->Render();
			gameSession->RenderBlit();
		}
		// tmp — might be decompiler bullshit
		bool tmp;
		// chapBool -- real name unknown
		if (chapBool && MenuEnabled(tmp) && tmp)
		{
			IntroduceChapter();
		}
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
		}
		else
		{
			sysEvent();
			zCInputCallback::GetInput();
			RenderFrame();
			// tmp — might be decompiler bullshit
			bool tmp;
			// chapBool -- real name unknown
			if (chapBool && MenuEnabled(tmp) && tmp)
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

void CGameManager::Done()
{
	if ( !dontStartGame )
	{
		zERROR::Message("B: GOTHIC: Exiting game ...");

		zCCacheBase::S_ClearCaches();
		sysKillWindowsKeys(0);

		bool fastExit = zCOption::Parm(zoptions, "ZNOFASTEXIT") == 0;
		if ( fastExit )
		{
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

		if ( !gameSession )
		{
			zERROR::Fatal("B: GameMan: Call GameSessionInit before GameSessionDone");
		}

		gameSession->Done();

		if (gameSession)
			delete gameSession;
		gameSession = 0;
		ogame = 0;

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

CGameManager::~CGameManager()
{
	if (gameSession)
	{
		zERROR::Warning("B: GMAN: gameSession is existing. Call CGameManager::Done() before!");
	}

	if ( videoPlayInGame )
	{
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
	   mov     [esp+0C4h+sysInfo.cpuLow],   700.0
	   mov     [esp+0C4h+sysInfo.cpuHigh],  1200.0
	   mov     [esp+0C4h+sysInfo.memLow],   256.0
	   mov     [esp+0C4h+sysInfo.memHigh],  512.0
	   mov     [esp+0C4h+sysInfo.gmemLow],  16.0
	   mov     [esp+0C4h+sysInfo.gmemHigh], 64.0
	   */

	sysInfo.ScoreNow();

	zerr.Separator("");

	sysInfo.ResetSettings(-1.0);

	zerr.Separator("");
}

void CGameManager::GameSessionInit()
{
	if ( gameSession )
		zFATAL("B: GameMan: Call GameSessionInit once, not twice!"); // 1047,

	gameSession = new oCGame;
	ogame = gameSession;

	gameSession->Init();
	gameSession->savegameManager = savegameManager;
	gameSession->SetGameInfo(gameSession, 0);
}

void CGameManager::Read_Savegame(int slot)
{
	if ( slot >= 0 ) {
		auto sav = savegameManager->GetSavegame(slot);
		if ( sav && !sav->saveIncompatible ) {
			if ( oCSavegameInfo::DoesSavegameExist(sav) ) {
				zINFO(4, "B: GMAN: Loading game from slot "_s + slot); //1548

				exitSession = 0;

				if ( !gameSession )
					zFATAL("B: GameMan: Call GameSessionInit before GameSessionReset() "); //1039,

				// auto sm = savegameManager;
				oCSavegameManager::ClearCurrent();

				gameSession->SetGameInfo(0);
				gameSession->LoadSavegame(sav->slot, 1);

				if ( zmusic )
					zmusic->Stop();

				playTime = sav->playTimeSeconds;

				time(&Time);
			}
		}
	}
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
