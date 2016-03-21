void oCGame::Init()
{
	oCarsten_StartUp();

	zCSession::Init();

	sysEvent();

	auto world = zfactory->CreateWorld();
	SetWorld(world);
	zcon.SetWorld(world);
	zcon.SetParser(zparser);

	WorldInit();

	savegameManager->ClearCurrent();

	zSTRING script;
	if ( !zgameoptions ) {
		script = zgameoptions->ReadString(zOPT_SEC_INTERNAL, "gameScript", 0);
	} else {
		script = zgameoptions->ReadString(zOPT_SEC_FILES, "Game", 0);
	}

	if (script.IsEmpty())
		script = "Content\\Gothic";

	zINFO(4,"N: GAME: Loading file " + script + ".src or .dat"); // 739

	LoadParserFile(script + ".src");

	sysEvent();

	oCNpc::InitStatics();

	guilds    = new oCGuilds();
	infoman   = new oCInfoManager(zparser);
	newsman   = new oCNewsManager();
	svmman    = new oCSVMManager();
	trademan  = new oCTradeManager(zparser);
	portalman = new oCPortalRoomManager();
	spawnman  = new oCSpawnManager();

	hpBar = new oCViewStatusBar();
	screen->InsertItem(hpBar, 0);
	hpBar->Init(100, 7800, 1.0);
	hpBar->SetTextures("BAR_back.tga", "BAR_tempmax.tga", "BAR_health.tga", "");
	hpBar->SetMaxRange(0.0, 30.0);
	screen->RemoveItem(hpBar);

	swimBar = new oCViewStatusBar();
	screen->InsertItem(swimBar, 0);
	swimBar->Init(2900, 7800, 1.0);
	swimBar->SetTextures("BAR_back.tga", "BAR_tempmax.tga", "BAR_misc.tga", "");
	screen->RemoveItem(swimBar);


	manaBar = new oCViewStatusBar();
	screen->InsertItem(manaBar, 0);
	manaBar->Init(5700, 7800, 1.0);
	manaBar->SetTextures("BAR_back.tga", "BAR_tempmax.tga", "BAR_mana.tga", "");
	manaBar->SetMaxRange(0.0, 30.0);
	screen->RemoveItem(manaBar);

	focusBar = new oCViewStatusBar(0, 0, 1000, 1000, 2);
	screen->InsertItem(focusBar, 0);
	focusBar->Init(500, 500, 0.75);
	focusBar->SetTextures("BAR_back.tga", "BAR_tempmax.tga", "BAR_health.tga", "");

	int sizex, sizey;
	focus->GetSize(sizex, sizey);
	int fontY = screen->FontY();

	focusBar->SetPos((2000 - sizex) / 2, fontY);
	screen->RemoveItem(focusBar);

	sysEvent();

	if (progressBar)
		progressBar->SetPercent(10, "");

	if ( zinput->GetDeviceConnected(2) )
		zinput->SetDeviceEnabled(2, 1);
	if ( zinput->GetDeviceConnected(3) )
		zinput->SetDeviceEnabled(3, 1);

	auto sym = zparser.GetSymbol("VIEW_TIME_PER_CHAR");
	if (val) {
		float val;
		sym->GetValue(val, 0);
		zCView::s_fViewTimePerChar = val;
	}
}

void oCGame::SetRangesByCommandLine()
{
	if (zoptions->Parm("CLIPRANGE"))
		cliprange = zoptions->ParmValueInt("CLIPRANGE");

	if (zoptions->Parm("FOGRANGE"))
		fogrange = zoptions->ParmValueInt("FOGRANGE");

	playerLightInt = zoptions->ReadInt(zOPT_SEC_GAME, "cameraLightRange", 0);

	if (zoptions->Parm("CAMLIGHTRANGE")) {
		playerLightInt = zoptions->ParmValueInt("CAMLIGHTRANGE");
		zoptions->WriteInt(zOPT_SEC_GAME, "cameraLightRange", playerLightInt, 1);
	}

	if (zoptions->Parm("LIGHTRANGE") && pl_light) {
		pl_lightval = zoptions->ParmValueInt("LIGHTRANGE");
		pl_light->SetRange(pl_lightval, 1);
	} else if (pl_light) {
		pl_lightval = 0.0;
	}

	SetRanges();
}

void oCGame::EnvironmentInit()
{
	if (GetWorld()->bspTree.bspTreeMode) {
		cliprange = 60000.0;
		fogrange = 15000.0;

		zCOLOR color;
		color[2] = 0x64;
		color[1] = 0x64;
		color[0] = 0x64;

		playerLightInt = 0;

		SetRanges();

		zrenderer->SetFogColor(color);
	} else {
		Release(pl_light);

		pl_light = new zCVobLight();
		pl_light->Move(200.0, 170.0, -150.0);
		pl_light->SetRange(100.0, 1);
		pl_light->lightData.lightColor[2] = 255;
		pl_light->lightData.lightColor[1] = 200;
		pl_light->lightData.lightColor[2] = 155;
		pl_light->lightData.flags &= 0xFE;

		playerLightInt = 5000;
	}

	SetRangesByCommandLine();
}
