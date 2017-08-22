#include <Gothic/Game/zSession.h>
#include <Gothic/Game/zConsole.h>
#include <Gothic/Game/zOptions.h>
#include <Gothic/Game/oSaveGame.h>
#include <Gothic/Game/zWorld.h>
#include <Gothic/Game/oGuilds.h>
#include <Gothic/Game/oInfo.h>
#include <Gothic/Game/oNews.h>
#include <Gothic/Game/oSVM.h>
#include <Gothic/Input/zInput.h>
#include <Gothic/Menu/oViewStatusBar.h>

#include <Gothic/Script/zParser.h>

#include <Gothic/System/System.h>

#include <Logging/Log.h>

// actually thiscall, but 'this' is unused
Cdecl<oCWorld*()> CreateWorld{0x76FCA0}; // 0x77ED20

constexpr float SPAWN_INSERTTIME_MAX = 1000.0;
struct oCSpawnManager {
	struct oSSpawnNode {
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
	auto* world = CreateWorld();
	reinterpret_cast<zCSession_vt*>(_vtab)->SetWorld(this, world);
	zcon.world = world;
	zcon.parser = &zparser;

	reinterpret_cast<zCSession_vt*>(_vtab)->WorldInit(this);

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

	reinterpret_cast<zCSession_vt*>(_vtab)->LoadParserFile(this, script + ".src");

	Log("Game", "Initializing misc");
	sysEvent();

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
