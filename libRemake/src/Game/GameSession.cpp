#include <Gothic/Game/zSession.h>
#include <Gothic/Game/zConsole.h>
#include <Gothic/Game/zOptions.h>
#include <Gothic/Game/oSaveGame.h>
#include <Gothic/Game/zWorld.h>
#include <Gothic/Game/oGuilds.h>
#include <Gothic/Game/oInfo.h>
#include <Gothic/Game/oNews.h>
#include <Gothic/Game/oSVM.h>
#include <Gothic/Game/zTimer.h>
#include <Gothic/Input/zInput.h>
#include <Gothic/Menu/oViewStatusBar.h>

#include <Gothic/Script/zParser.h>

#include <Gothic/System/System.h>

#include <Logging/Log.h>
#include <Gothic/Game/oNpc.h>

constexpr float SPAWN_INSERTTIME_MAX = 1000.0;
struct oCSpawnManager {
	static float GetSpawnTime();
	void CheckInsertNpcs();
	
	void ClearList()
	{
		spawnList.DeleteList();
	}

	struct oSSpawnNode {
		~oSSpawnNode()
		{
			npc->Release();
		}

		oCNpc *npc;
		zVEC3 pos;
		float time;
	};

	zCArray<oSSpawnNode> spawnList;
	int spawningEnabled = true;
	zVEC3 camPos{0,0,0};
	float insertTime = SPAWN_INSERTTIME_MAX;
	int spawnFlags = 0;
};

float oCSpawnManager::GetSpawnTime()
{
	//SPAWN_INSERTTIME_MAX
	return Value<float>(0x8BACCC);
}

void oCSpawnManager::CheckInsertNpcs()
{
	if ( spawningEnabled ) {
		insertTime += ztimer.FrameTime();
		if ( insertTime >= GetSpawnTime() ) {
			insertTime -= GetSpawnTime();
			Thiscall<void(oCSpawnManager*)> InitCameraPos{0x7788C0};
			Thiscall<void(oCSpawnManager*)> CheckInsertNpc{0x7780B0};
			InitCameraPos(this);
			CheckInsertNpc(this);
		}
	}
}

struct oCTradeManager {
	oCTradeManager(zCParser& par)
	{
		Thiscall<void(oCTradeManager*, zCParser&)> ctor{0x7147E0};
		ctor(this, par);
	}
	zCParser *parser;
	int unk[4];
};

struct oCPortalRoom;
struct oCPortalRoomManager {
	oCPortalRoomManager()
	{
		portals.Compare = reinterpret_cast<zCArraySort<oCPortalRoom*>::CompareFunc*>(0x772620);
	}
	zSTRING *oldPlayerPortal = nullptr;
	zSTRING *curPlayerPortal = nullptr;
	oCPortalRoom *oldPlayerRoom = nullptr;
	oCPortalRoom *curPlayerRoom = nullptr;
	zCArraySort<oCPortalRoom*> portals;
};


static void __thiscall Render_thunk(zCSession* ptr) { static_cast<oCGame*>(ptr)->Render(); }
static int __thiscall LoadParserFile_thunk(oCGame* game, zSTRING const& script)
{
	return game->LoadParserFile(std::string(script));
}
static void __thiscall WorldInit_thunk(oCGame* game) { game->WorldInit(); }

static zCSession_vt oGameVt;
static zCSession_vt* init_vt(zCSession_vt* from)
{
	if (zoptions->ReadBool("REMAKE", "use_default_ogame", false))
		return from;
	oGameVt = *from;
	oGameVt.Render = Render_thunk;
	oGameVt.LoadParserFile = LoadParserFile_thunk;
	oGameVt.WorldInit = WorldInit_thunk;
	return &oGameVt;
}

static int Sort_Routine(TObjectRoutine* entry1, TObjectRoutine* entry2)
{
	if (entry1->hour > entry2->hour)
		return 1;

	if (entry1->hour == entry2->hour && entry1->min >= entry2->min)
		return 1;

	return -1;
}

#include <Hook/size_checker.h>
CHECK_SIZE(zCSession, 0x1C);
CHECK_SIZE(oCGame, 0x18C);

#include <aw/utility/string/split.h>
#include <Types/string_view.h>
#include <Utility/String.h>
oCGame::oCGame() : zCSession()
{
	static zCSession_vt* cvt = init_vt((zCSession_vt*)(0x83C28C));
	_vtab = cvt;

	for (auto& v : array_view_enabled)
		v = 1;

	game_testmode = zoptions->Parm("DEVMODE");
	game_testmode = true; // TODO: remove

	objRoutineList.compareFunc = (void*)(0x06CA930)/*Sort_Routine*/;

	SetEnableHandleEvent(true);

	worldEntered    = 0;
	enterWorldTimer = 0;

	debugChannels     = -(zoptions->ReadBool("internal", "debugAllChannels", 0));
	debugAllInstances = zoptions->ReadBool("internal", "debugAllInstances", 0);
	if ( !debugChannels ) {
		using namespace std::string_view_literals;

		auto channels = zoptions->ReadString("internal", "debugChannels", 0);

		auto words = aw::string::split_by(channels, ",; "sv);
		for (auto word : words)
		{
			long chan = parse(word, 0l);
			if ( chan >= 1 && chan <= 32 )
				debugChannels |= 1 << (chan - 1);
		}
	}
}

void oCGame::Init()
{
	using namespace g2;

	Log("Game", "Initializing game");
	// oCarsten_StartUp(); //duplicate

	reinterpret_cast<zCSession_vt*>(_vtab)->DesktopInit(this);

	sysEvent();

	reinterpret_cast<zCSession_vt*>(_vtab)->CutsceneSystemInit(this);

	sysEvent();

	Log("Game", "Creating world");
	auto* world = new oCWorld;
	reinterpret_cast<zCSession_vt*>(_vtab)->SetWorld(this, world);
	zcon.world = world;
	zcon.parser = &zparser;

	WorldInit();

	savegameManager->ClearCurrent();

	auto GetGameScript = [] {
		if ( !zgameoptions ) {
			return zoptions->ReadString("INTERNAL", "gameScript", 0);
		} else {
			return zgameoptions->ReadString("FILES", "Game", 0);
		}
	};
	std::string script = GetGameScript();

	if (script.empty())
		script = "Content/Gothic";

	Log("Game", "Loading game script: " + script);

	LoadParserFile(script + ".src");

	Log("Game", "Initializing misc");
	sysEvent();
	Cdecl<void()> oCNpc__InitStatics{0x75E4D0};
	oCNpc__InitStatics();

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
	focusBar->GetSize(sizex, sizey);
	int fontY = screen->FontY();

	focusBar->SetPos((2000 - sizex) / 2, fontY);
	screen->RemoveItem(focusBar);

	sysEvent();

	if (progressBar)
		progressBar->SetPercent(10, "");

	if ( zinput->GetDeviceConnected(DEVICE_MOUSE) )
		zinput->SetDeviceEnabled(DEVICE_MOUSE, 1);
	if ( zinput->GetDeviceConnected(DEVICE_JOYSTICK) )
		zinput->SetDeviceEnabled(DEVICE_JOYSTICK, 1);

	if (auto sym = zparser.GetSymbol("VIEW_TIME_PER_CHAR")) {
		float val = sym->GetValue<float>(0);

		auto& zCView__viewTimePerChar = Value<float>(0x8BC868);
		zCView__viewTimePerChar = val;
	}
	Log("Game", "Initializing done");
}

#include <Gothic/Game/oWorldTimer.h>
#include <Gothic/Game/oRtnManager.h>
void oCGame::WorldInit()
{
	g2::Log("Game", "WorldInit");
	world->bspTree.drawVobBBox3D = 0;

	wldTimer = new oCWorldTimer();

	initial_hour = 8;
	initial_minute = 0;


	if (  zoptions->Parm("TIME") ) {
		using namespace std::string_view_literals;

		auto time = zoptions->ParmValue("TIME");

		auto parts = aw::string::split_by(time, " :"sv);
		if ( parts.size() > 1 )
		{
			try_parse(std::string(parts[0]), initial_hour);
			try_parse(std::string(parts[1]), initial_hour);
		}
	}

	rtnMan.SetWorld(world);
}


#include <Gothic/Graphics/zRenderer.h>
#include <Gothic/Graphics/zSkyControler.h>


struct zCCamera {
	void Activate()
	{
		thiscall(0x54A700, this);
	}
};

struct zCResourceManager {
	void DoFrameActivity()
	{
		thiscall(0x5DD4F0, this);
	}
};

struct zCViewDraw {
	void Render()
	{
		thiscall(0x6900E0, this);
	}
};

inline auto& zresMan = Value<zCResourceManager*>(0x99AB30);
void oCGame::Render()
{
	using namespace g2;
	auto AdvanceClock = [] (float step) {
		if ( step >= 0.0 ) {
			ztimer.SetFrameTime(step * 1000.0);
		} else {
			ztimer.ResetTimer();
		}
	};

	if ( loadNextLevel ) {
		Log("Game::Render", "Loading level" + loadNextLevelName);

		Thiscall<void(oCGame*, zSTRING const&, zSTRING const&)> ChangeLevel{0x6C7290};
		ChangeLevel(this, loadNextLevelName, loadNextLevelStart);

		Log("Game::Render", "Level loaded");
	}

	if ( worldEntered && !game_testmode ) {
		zCTimer::FrameUpdate();
		AdvanceClock(timeStep);

		camera->Activate();

		world->MoveVobs();

		if ( zresMan )
			zresMan->DoFrameActivity();

		spawnman->CheckInsertNpcs();

		enterWorldTimer += ztimer.FrameTime();
		if ( enterWorldTimer >= 2500.0 ) {
			worldEntered = 0;
			enterWorldTimer = 0;
		}
		return;
	}

	zrenderer->BeginFrame();

	auto game_holdtime = Value<int>(0xAB0888);
	if ( !game_holdtime )
		wldTimer->Timer();

	auto skyCtl = world->GetActiveSkyControler();

	skyCtl->SetTime(wldTimer->GetSkyTime());

	Cdecl<void()> oCVisualFX__SetupAIForAllFXInWorld{0x4898B0};
	oCVisualFX__SetupAIForAllFXInWorld();

	world->Render(camera);


	spawnman->CheckInsertNpcs();


	Cdecl<void()> oCNpc__ProcessAITimer{0x75F360};
	oCNpc__ProcessAITimer();


	if ( game_drawall ) {
		RenderWaynet();
		//ShowDebugInfos();
		Thiscall<void(oCGame*)> UpdatePlayerStatus{0x6C3140};
		UpdatePlayerStatus(this);

		screen->DrawItems();
		Cdecl<void()> oCItemContainer__Container_Draw{0x704B90};
		oCItemContainer__Container_Draw();

		/*if ( drawWayBoxes )
		  rtnMan->DrawWayBoxes();
		  if ( showFreePoints )
		  ShowFreePoints();*/

		static auto& infoman = oCInformationManager::GetInformationManager();
		if ( infoman.WaitingForEnd() )
			infoman.Update();

		Cdecl<zCViewDraw*()> GetScreen{ 0x6905C0 };
		GetScreen()->Render();
	}

	//GetCamera()->ShowVobDebugInfo();

	zrenderer->Vid_Unlock();

	Cdecl<void()> oCarsten_PerFrame{0x4816C0};
	oCarsten_PerFrame();

	Thiscall<void(oCGame*)> CheckObjectRoutines{0x6CABB0};
	CheckObjectRoutines(this);

	zCTimer::FrameUpdate();
	AdvanceClock(timeStep);

	zrenderer->EndFrame();

	if ( singleStep )
		timeStep = 0;
}

void DefineExternals2(zCParser&);

bool oCGame::LoadParserFile(std::string const& fileName)
{
	g2::Log("Game::Parser", "Loading parser file ", fileName);

	Cdecl<void(zCParser&)> DefineExternals_Ulfi{0x6D4780};

	zparser.Reset();
	DefineExternals2(zparser);
	DefineExternals_Ulfi(zparser);

	zparser.EnableTreeLoad(0);
	zparser.EnableTreeSave(0);

	zparser.Parse(fileName);

	zparser.AddClassOffset("C_NPC", 288);
	zparser.AddClassOffset("C_ITEM", 288);

	zparser.CreatePCode();

	g2::Log("Game::Parser", "Parsing done");

	return !zparser.Error();
}

void oCGame::LoadGame(int slotID, std::string const& levelpath)
{
	g2::Log("Game", "Loading game: slot ", slotID, " level ", levelpath);

	//thiscall(0x6C65A0, this, slotID, as<zSTRING const&>(levelpath));
	//return;

	ClearGameState();

	OpenLoadscreen(true, levelpath);

	if ( progressBar )
	{
		progressBar->SetPercent(0, "");
		progressBar->SetRange(0, 92);
	}

	LoadWorld(slotID, levelpath);

	if ( progressBar )
	{
		progressBar->ResetRange();
		progressBar->SetRange(92, 100);
	}

	EnterWorld(nullptr, true, "");

	if ( progressBar )
		progressBar->ResetRange();

	if ( slotID == SAVEGAME_SLOT_NEW )
		InitNpcAttitudes();

	SetTime(0, initial_hour, initial_minute);

	if ( progressBar )
		progressBar->SetPercent(100, "");

	CloseLoadscreen();
}

#include <Gothic/Game/zWayNet.h>
void oCGame::LoadWorld(int slotID, std::string_view levelpath)
{
	g2::Log("Game", "Loading world: ", levelpath);

	if (levelpath.empty()) {
		g2::Warning("Game", "fileName is empty");
		return;
	}

	if ( progressBar )
		progressBar->SetPercent(12, "");

	if ( slotID == SAVEGAME_SLOT_NEW ) {
		spawnman->ClearList();
		world->DisposeWorld();

		if ( progressBar )
			progressBar->SetRange(15, 97);

		LoadWorldStartup(levelpath);

		if ( progressBar )
			progressBar->ResetRange();

		if ( !world->compiled )
			CompileWorld();
	} else {
		std::string_view fileName = world->worldFilename;
		if (fileName != levelpath) {
			if ( progressBar )
				progressBar->SetRange(15, 40);

			LoadWorldStat(fileName);

			if ( progressBar )
				progressBar->ResetRange();
		}

		if ( progressBar )
			progressBar->SetPercent(42, "");

		if ( !world->compiled )
			CompileWorld();

		inLoadSaveGame = 1;
		if ( progressBar )
			progressBar->SetRange(44, 97);

		LoadWorldDyn(levelpath);

		if ( progressBar )
			progressBar->ResetRange();

		inLoadSaveGame = 0;
	}

	world->wayNet->CorrectHeight();

	// TODO: don't care for now, as sound is only half-implemented
	/*auto skyCtrl = world->GetActiveSkyControler();
	if (zDYNAMIC_CAST<zCSkyControler_Indoor>(skyCtrl))
		zsound->SetGlobalReverbPreset(8, 0.4);
	else
		zsound->SetGlobalReverbPreset(0, 0.0);*/

	if ( progressBar )
		progressBar->SetPercent(100, "");
}



void oCGame::LoadWorldStartup(std::string_view levelpath)
{
	g2::Log("U: GAM:", "Loading startup-data: \"", levelpath, "\"...");

	if ( progressBar )
		progressBar->SetRange(0, 75);

	world->LoadWorld(levelpath, zWLD_LOAD_GAME_STARTUP);

	if ( progressBar )
		progressBar->ResetRange();

	g2::Log("U: GAM:", "Loading startup-data \"", levelpath, "\" finished.");

	if ( progressBar )
		progressBar->SetPercent(76, "");

	g2::Log("U: GAM:", "Cleaning world...");

	RemoveHelperVobs(world->globalVobTree.firstChild);

	g2::Log("U: GAM:", "GAM: .. finished");

	if ( progressBar )
		progressBar->SetPercent(79, "");

	if ( progressBar )
		progressBar->SetRange(80, 100);

	CallScriptStartup();
	CallScriptInit();

	if ( progressBar )
		progressBar->ResetRange();
}


void oCGame::CallScriptStartup()
{
	g2::Log("Game", "Startup ", std::string_view(world->worldName));

	if ( !scriptStartup )
		return;

	auto worldName = std::string(world->worldName);

	auto startup_global = zparser.GetIndex("STARTUP_GLOBAL");
	auto startup_world  = zparser.GetIndex("STARTUP_" + worldName);
	
	if (startup_global <= 0)
		g2::Fatal("U: GAM", "Global Startup Function not found.");

	inScriptStartup = 1;

	zparser.CallFunc(startup_global);

	if ( startup_world > 0 ) {
		g2::Log("U: GAM", "Calling Startup-Script ...");

		zparser.SetProgressBar(progressBar);
		zparser.CallFunc(startup_world);
		zparser.SetProgressBar(0);

		g2::Log("U: GAM", "Startup-Script finished.");
	} else {
		g2::Warning("Game", "STARTUP_", worldName, " not found");
	}

	inScriptStartup = 0;

	g2::Log("Game", "Startup end");
}

void oCGame::CallScriptInit()
{
	auto worldName = std::string(world->worldName);

	auto init_global = zparser.GetIndex("INIT_GLOBAL");
	auto init_world  = zparser.GetIndex("INIT_" + worldName);

	if (init_global <= 0)
		g2::Fatal("U:GAM", "Global Init Function not found.");

	inScriptStartup = 1;

	zparser.CallFunc(init_global);

	if ( init_world > 0 ) {
		g2::Log("U: GAM", "Calling Init-Script ...");

		zparser.SetProgressBar(progressBar);
		zparser.CallFunc(init_world);
		zparser.SetProgressBar(0);

		g2::Log("U: GAM", "Init-Script finished.");
	}

	inScriptStartup = 0;
}

void oCGame::RenderWaynet()
{
	if ( game_showwaynet )
	{
		if ( world->wayNet )
		{
			world->wayNet->Draw(camera);
		}
	}
}


