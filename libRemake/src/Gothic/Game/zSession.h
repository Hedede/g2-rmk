#ifndef Gothic_zSession_H
#define Gothic_zSession_H
#include <Gothic/Input/zCInputCallback.h>
#include <Gothic/Types/zSTRING.h>
#include <Gothic/Types/zTree.h>
#include <Hook/Externals.h>

struct zCCSManager;
struct zCWorld;
struct oCWorld;
struct zCCamera;
struct zCAICamera;
struct zCVob;
struct oCNpc;

struct zCView;

struct zCSession;
struct oCGame;
struct oCGameInfo;


struct zCSession_vt : zCInputCallback_vt {
	// zCSession:
	void (__thiscall *dtor)(zCSession *);
	void (__thiscall *Init)(zCSession *);
	void (__thiscall *Done)(zCSession *);
	void (__thiscall *Render)(zCSession *);
	void (__thiscall *RenderBlit)(zCSession *);
	void (__thiscall *CamInit1)(zCSession *, zCVob *, zCCamera *);
	void (__thiscall *CamInit2)(zCSession *);
	void (__thiscall *SetCamera)(zCSession *, zCCamera *);
	zCCamera *(__thiscall *GetCamera)(zCSession *);
	zCAICamera *(__thiscall *GetCameraAI)(zCSession *);
	zCVob *(__thiscall *GetCameraVob)(zCSession *);
	zCView *(__thiscall *GetViewport)(zCSession *);
	zCWorld *(__thiscall *GetWorld)(zCSession *);
	void (__thiscall *SaveWorld)(zCSession *, const zSTRING *, int, int, int);
	void (__thiscall *LoadWorld)(zCSession *, const zSTRING *, int);
	zCCSManager *(__thiscall *GetCutsceneManager)(zCSession *);
	void (__thiscall *SetTime)(zCSession *, int, int, int);
	void (__thiscall *GetTime)(zCSession *, int *, int *, int *);
	void (__thiscall *SetWorld)(zCSession *, zCWorld *);
	void (__thiscall *DesktopInit)(zCSession *);
	void (__thiscall *CutsceneSystemInit)(zCSession *);

	// oCGame:
	void (__thiscall *EnterWorld)(oCGame *, oCNpc *, int, const zSTRING&);
	void (__thiscall *Pause)(oCGame *, int);
	void (__thiscall *Unpause)(oCGame *);
	void (__thiscall *SetDrawWaynet)(oCGame *, int);
	void (__thiscall *GetDrawWaynet)(oCGame *);
	void (__thiscall *RenderWaynet)(oCGame *);
	void (__thiscall *EnvironmentInit)(oCGame *);
	void (__thiscall *SetRanges)(oCGame *);
	void (__thiscall *SetRangesByCommandLine)(oCGame *);
	void (__thiscall *GetStartPos)(oCGame *);
	void (__thiscall *SetGameInfo)(oCGame *, oCGameInfo *);
	int (__thiscall *LoadParserFile)(oCGame *, const zSTRING&);
	void (__thiscall *TriggerChangeLevel)(oCGame *, const zSTRING *, const zSTRING *);
	oCWorld *(__thiscall *GetGameWorld)(oCGame *);
	void (__thiscall *GetGameInfo)(oCGame *);
	zCView *(__thiscall *GetTextView)(oCGame *);
	void (__thiscall *OpenLoadscreen)(oCGame *, bool, zSTRING);
	void (__thiscall *OpenSavescreen)(oCGame *, bool);
	void (__thiscall *CloseLoadscreen)(oCGame *);
	void (__thiscall *CloseSavescreen)(oCGame *);
	void (__thiscall *ChangeLevel)(oCGame *, const zSTRING *, const zSTRING *);
	void (__thiscall *LoadWorldStartup)(oCGame *, const zSTRING *);
	void (__thiscall *LoadWorldStat)(oCGame *, zSTRING);
	void (__thiscall *LoadWorldDyn)(oCGame *, const zSTRING&);
	void (__thiscall *InitWorldSavegame)(oCGame *, int *, zSTRING *);
	int (__thiscall *CheckIfSavegameExists)(oCGame *, const zSTRING *);
	void (__thiscall *CompileWorld)(oCGame *);
	void (__thiscall* WorldInit)(oCGame*);
	void (__thiscall *NpcInit1)(oCGame *, void *); // zCTree<zCVob*>
	void (__thiscall *NpcInit2)(oCGame *);
	void (__thiscall *SetAsPlayer)(oCGame *, const zSTRING *);
};


struct zCSession : zCInputCallback {
	zCSession() : zCInputCallback()
	{
		_vtab = reinterpret_cast<void*>(0x832C4C);
	}

	zCSession_vt& vtab()
	{
		return *reinterpret_cast<zCSession_vt*>(_vtab);
	}

	zCCamera* GetCamera()
	{
		return camera;
	}

	void Render()
	{
		reinterpret_cast<zCSession_vt*>(_vtab)->Render(this);
	}

	void RenderBlit()
	{
		reinterpret_cast<zCSession_vt*>(_vtab)->RenderBlit(this);
	}

	oCWorld* GetWorld()
	{
		return world;
	}


	zCCSManager*  csMan    = nullptr;
	oCWorld*      world    = nullptr;
	zCCamera*     camera   = nullptr;
	zCAICamera*   aiCam    = nullptr;
	zCVob*        camVob   = nullptr;
	zCView*       viewport = nullptr;
};


#include <Gothic/Types/zVEC3.h>
#include <Gothic/Types/zArray.h>

struct TObjectRoutine {
	~TObjectRoutine() = default;

	void Release()
	{
		delete this;
	}

	zSTRING objectName;
	int hour;
	int min;
	int state;
	int type;
};

struct TObjectRoutineList {
	void *compareFunc;
	TObjectRoutine *data     = nullptr;
	TObjectRoutineList *next = nullptr;
};

struct oCSavegameManager;
struct zCViewProgressBar;
struct oCViewStatusBar;
struct oCNewsManager;
struct oCGuilds;
struct oCSVMManager;
struct oCTradeManager;
struct oCInfoManager;
struct oCPortalRoomManager;
struct oCSpawnManager;
struct zCVisual;
struct zCVobLight;
struct oCWorldTimer;

constexpr int GAME_VIEW_MAX = 6;
struct oCGame : zCSession {
	oCGame();

	void Init();
	void WorldInit();

	void Render();
	void RenderWaynet();

	void SetGameInfo(oCGameInfo* info)
	{
		reinterpret_cast<zCSession_vt*>(_vtab)->SetGameInfo(this, info);
	}

	bool LoadParserFile(std::string const& script);

	void SetShowPlayerStatus(bool show)
	{
		Thiscall<void(oCGame*,int)> call{0x6C2D90};
		call(this,show);
	}

	void Pause(int noscreen)
	{
		vtab().Pause(this, noscreen);
	}

	void Unpause()
	{
		vtab().Unpause(this);
	}

	void OpenLoadscreen(bool gameStart, std::string const& level)
	{
		Thiscall<void(oCGame*, int, zSTRING)> call(0x6C2690);
		call(this, gameStart, level);
	}

	void CloseLoadscreen()
	{
		thiscall(0x6C2BD0, this);
	}

	void UpdateScreenResolution()
	{
		thiscall(0x6C2E00, this);
	}

	void ClearGameState()
	{
		thiscall(0x6C5ED0, this);
	}

	void InitNpcAttitudes()
	{
		thiscall(0x6C61D0, this);
	}

	void CompileWorld()
	{
		vtab().CompileWorld(this);
	}


	void LoadGame(int slotID, std::string const& level); // 6C65A0

	void LoadWorld(int slotID, std::string_view levelpath);
	void LoadWorldStartup(std::string_view levelpath);
	void LoadWorldStat(std::string_view levelpath)
	{
		vtab().LoadWorldStat(this,levelpath);
	}
	void LoadWorldDyn(std::string_view levelpath)
	{
		vtab().LoadWorldDyn(this,levelpath);
	}

	void EnterWorld(oCNpc* playerVob, bool changePlayerPos, std::string const& startPoint)
	{
		vtab().EnterWorld(this, playerVob, changePlayerPos, startPoint);
		/*Thiscall<void(oCGame*, oCNpc*, int, const zSTRING&)> call{0x6C96F0};
		call(this, playerVob, changePlayerPos, startPoint);

		thiscall(0x6C4DE0, this, as<int>(changePlayerPos), as<const zSTRING&>(startPoint));*/
	}

	void SetTime(int day, int hour, int min)
	{
		thiscall(0x6C4DE0, this, day, hour, min);
	}

	void RemoveHelperVobs(zCTree<zCVob> *node)
	{
		stdcall(0x6C3C10, node);
	}

	void CallScriptStartup();
	void CallScriptInit();

	oCSpawnManager* GetSpawnManager()
	{
		return spawnman;
	}

public:
	float cliprange = 1600.0;
	float fogrange  = 400.0;

	int inScriptStartup = 0;
	int inLoadSaveGame  = 0;
	int inLevelChange   = 0;

	zCView *array_view[GAME_VIEW_MAX]     = {};
	int array_view_visible[GAME_VIEW_MAX] = {};
	int array_view_enabled[GAME_VIEW_MAX] = {};

	oCSavegameManager *savegameManager = nullptr;

	zCView *game_text         = nullptr;
	zCView *load_screen       = nullptr;
	zCView *save_screen       = nullptr;
	zCView *pause_screen      = nullptr;
	oCViewStatusBar *hpBar    = nullptr;
	oCViewStatusBar *swimBar  = nullptr;
	oCViewStatusBar *manaBar  = nullptr;
	oCViewStatusBar *focusBar = nullptr;

	int showPlayerStatus  = 1;
	int game_drawall      = 1;
	int game_frameinfo    = 0;
	int game_showaniinfo  = 0;
	int game_showwaynet   = 0;
	int game_testmode     = 0;
	int game_editwaynet   = 0;
	int game_showtime     = 0;
	int game_showranges   = 0;
	int drawWayBoxes      = 0;
	int scriptStartup     = 1;
	int showFreePoints    = 0;
	int showRoutineNpc    = 0;

	int     loadNextLevel     = 0;
	zSTRING loadNextLevelName;
	zSTRING loadNextLevelStart;

	zVEC3 startpos;

	oCGameInfo *gameInfo = nullptr;

	zCVobLight *pl_light    = nullptr;
	float       pl_lightval = 5000.0;

	oCWorldTimer *wldTimer = nullptr;

	float timeStep = -1.0;
	int singleStep = 0;

	oCGuilds*            guilds    = nullptr;
	oCInfoManager*       infoman   = nullptr;
	oCNewsManager*       newsman   = nullptr;
	oCSVMManager*        svmman    = nullptr;
	oCTradeManager*      trademan  = nullptr;
	oCPortalRoomManager* portalman = nullptr;
	oCSpawnManager*      spawnman  = nullptr;

	int music_delay = 0;
	oCNpc *watchnpc = 0;

	int worldEntered    = 0;
	int enterWorldTimer = 0;

	int initial_hour    = 0;
	int initial_minute  = 0;

	zCArray<zCVob*> debugInstances;

	int debugChannels     = 0;
	int debugAllInstances = 0;

	int oldRoutineDay = -1;
	TObjectRoutineList objRoutineList;
	TObjectRoutineList *currentObjectRoutine = nullptr;

	zCViewProgressBar *progressBar = nullptr;

	zCArray<zCVisual*> visualList;
};


#endif//Gothic_zSession_H
