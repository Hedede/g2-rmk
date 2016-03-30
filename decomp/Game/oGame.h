//statics:
const int GAME_VIEW_MAX = 6;

class oCGame : public zCSession {
public:
	static oCNpc* GetSelfPlayerVob()
	{
		return oCNpc::player;
	}

	oCGame();
	virtual ~oCGame();

	virtual bool HandleEvent(int key);

	virtual void Init();
	virtual void Done();

	virtual void Render();
	virtual void RenderBlit();

	virtual void SaveWorld(zSTRING const& dwf, zCWorld::zTWorldSaveMode,int,int);
	virtual void LoadWorld(zSTRING const& dwf, zCWorld::zTWorldLoadMode);

	virtual void EnterWorld(oCNpc *,int,zSTRING const &);

	virtual void DesktopInit();
	virtual void EnvironmentInit();

	virtual void Pause(int);
	virtual void Unpause();

	virtual void SetDrawWaynet(bool b)
	{
		game_showwaynet = b;
	}

	virtual bool GetDrawWaynet() const
	{
		return game_showwaynet;
	}
	virtual void RenderWaynet();

	virtual void SetRanges();
	virtual void SetRangesByCommandLine();

	virtual zVEC3 GetStartPos() const
	{
		return startpos;
	}
	virtual void SetGameInfo(oCGameInfo *);
	virtual void LoadParserFile(zSTRING const &);
	virtual void TriggerChangeLevel(zSTRING const &,zSTRING const &);

	virtual oCWorld* GetGameWorld() const;

	virtual oCGameInfo& GetGameInfo()
	{
		return gameInfo;
	}

	virtual zCView* GetTextView();

	virtual void OpenLoadscreen(bool,zSTRING);
	virtual void OpenSavescreen(bool);

	virtual void CloseLoadscreen();
	virtual void CloseSavescreen();

	virtual void ChangeLevel(zSTRING const &,zSTRING const &);
	virtual void LoadWorldStartup(zSTRING const &);
	virtual void LoadWorldStat(zSTRING);
	virtual void LoadWorldDyn(zSTRING const &);
	virtual void InitWorldSavegame(int &,zSTRING &);

	virtual void CheckIfSavegameExists(zSTRING const &);
	virtual void CompileWorld();

	virtual void WorldInit();

	virtual void SetTime(int day, int hour, int min);
	virtual void GetTime(int& day, int& hour, int& min) const
	{
		day = wldTimer->GetDay(wldTimer);
		wldTimer->GetTime(hour, min);
	}

private:virtual void NpcInit(zCTree<zCVob>* vobtree);
public: virtual void NpcInit();

	virtual void SetAsPlayer(zSTRING const &);

	oCNewsManager& GetNewsManager()
	{
		return newsman;
	}

	oCInfoManager& GetInfoManager()
	{
		return infoman;
	}

	oCGuilds& GetGuilds()
	{
		return guilds;
	}

	int IsDebuggingAllInstances() const
	{
		return debugAllInstances;
	}

	oCWorldTimer* GetWorldTimer() const
	{
		return wldTimer;
	}

	oCTradeManager* GetTradeManager() const
	{
		return trademan;
	}

	zCView* GetTextView() const
	{
		return game_text;
	}

	oCInfoManager* GetInfoManager() const
	{
		return infoman;
	}

	oCSVMManager* GetSVMManager() const
	{
		return svmman;
	}

	oCPortalRoomManager* GetPortalRoomManager(oCGame *this) const
	{
		return portalman;
	}

	oCSpawnManager* GetSpawnManager(oCGame *this) const
	{
		return spawnman;
	}

	int GetShowPlayerStatus(oCGame *this) const
	{
		return showPlayerStatus;
	}

	void ToggleShowFreePoints(oCGame *this)
	{
		showFreePoints = !showFreePoints;
	}


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

	bool IsDebuggingChannel(int channel) const
	{
		return (debugChannels & (1 << (channel - 1))) != 0;
	}

	bool IsDebuggingInstance(zCVob* vob) const
	{
		return debugInstances.IsInList(vob);
	}

private:
	void SetCameraPosition();
	oCNpc* RemovePlayerFromWorld();
	void InsertPlayerIntoWorld(oCNpc* npc, zMAT4& trafo);

	int IsInWorld(zSTRING& levelpath);

	static int Sort_Routine(TObjectRoutine* entry1, TObjectRoutine* entry2)
	{
		if (entry1->hour >= entry2->hour) {
			if (entry1->hour1 > entry2->hour || entry1->min >= entry2->min)
				return 1;
			return -1;
		}

		return -1;
	}

	static void CreateVobList(zCList<zCVob>& resultList, zCTree<zCVob>& node, zTVobType vobtyp)
	{
		for (auto i = &node; i; i = i->next) {
			if ( i->data->type == vobtyp )
				resultList.Insert(i->data);
			CreateVobList(resultList, i->firstChild, vobtyp);
		}
	}

	void CacheVisualsOut();

	void PreSaveGameProcessing(int saveGlobals);
	void PostSaveGameProcessing();
private:
	zREAL  cliprange = 1600.0;
	zREAL  fogrange  = 400.0;
	zBOOL  inScriptStartup = 0;
	zBOOL  inLoadSaveGame  = 0;
	zBOOL  inLevelChange   = 0;

	enum oTGameDialogView {
		GAME_VIEW_SCREEN,
		GAME_VIEW_CONVERSATION,
		GAME_VIEW_AMBIENT,
		GAME_VIEW_CINEMA,
		GAME_VIEW_CHOICE,
		GAME_VIEW_NOISE,
		GAME_VIEW_MAX
	};

	//Views sind Kanäle, über die die Engine
	//Informationen anzeigen kann.
	//Beispiel sind die normalen Dialoguntertitelboxen.
	zCView*  array_view[GAME_VIEW_MAX]         = {};
	zBOOL    array_view_visible[GAME_VIEW_MAX] = {};
	zBOOL    array_view_enabled[GAME_VIEW_MAX] = {};

	oCSavegameManager*  savegameManager = 0;
	zCView*             game_text    = 0;
	zCView*             load_screen  = 0;
	zCView*             save_screen  = 0;
	zCView*             pause_screen = 0;
	oCViewStatusBar*    hpBar     = 0;
	oCViewStatusBar*    swimBar   = 0;
	oCViewStatusBar*    manaBar   = 0;
	oCViewStatusBar*    focusBar  = 0;
	zBOOL               showPlayerStatus = 1;

	// Debugsachen
	zBOOL  game_drawall      = 1; //"toggle Desktop"
	zBOOL  game_frameinfo    = 0;
	zBOOL  game_showaniinfo  = 0;
	zBOOL  game_showwaynet   = 0;
	zBOOL  game_testmode     = 0;
	zBOOL  game_editwaynet   = 0;
	zBOOL  game_showtime     = 0;
	zBOOL  game_showranges   = 0;
	zBOOL  drawWayBoxes      = 0;
	zBOOL  scriptStartup     = 1;
	zBOOL  showFreePoints    = 0;
	oCNpc* showRoutineNpc    = 0;

	// Levelinfos
	zBOOL   loadNextLevel = 0;
	zSTRING loadNextLevelName;
	zSTRING loadNextLevelStart;

	// Spielerspezifika
	zVEC3 startpos;
	//etwas auf den ersten Blick uninteressantes
	oCGameInfo* gameInfo = 0;

	zCVobLight*  pl_light = 0;
	zREAL        pl_lightval = 5000.0;

	// Timer
	oCWorldTimer* wldTimer = 0;
	// Ermöglicht Einzelbildschaltung
	zREAL timeStep = -1.0;
	zBOOL singleStep = 0;

	// Referenzen auf Einzelstückklassen.
	oCGuilds*            guilds    = 0;
	oCInfoManager*       infoman   = 0;
	oCNewsManager*       newsman   = 0;
	oCSVMManager*        svmman    = 0;
	oCTradeManager*      trademan  = 0;
	oCPortalRoomManager* portalman = 0;
	oCSpawnManager*      spawnman  = 0;

	//Zeug
	zREAL  music_delay = 0;
	oCNpc* watchnpc    = 0;

	//Kurz nachdem Laden kurze Pause, damit
	//sich die Systemressourcen erholen.
	zBOOL  worldEntered;
	zREAL  enterWorldTimer;

	//Klar:
	int initial_hour;
	int initial_minute;

	//Debug:
	zCArray<zCVob*>           debugInstances;

	int debugChannels;
	int debugAllInstances;

	//Objektroutinen ("Wld_SetObjectRoutine"), z.B. Laternen: nur Nachts an
	typedef struct {
		zSTRING     objName;
		int         stateNum;
		int         hour1,min1;
		int         type;
	} TObjectRoutine;

	int oldRoutineDay = -1;

	zCListSort<TObjectRoutine>    objRoutineList;

	zCListSort<TObjectRoutine>* currentObjectRoutine = 0;

	//ProgressBar
	zCViewProgressBar* progressBar;

	//Nicht jedes Fass in der Welt hat ein privates Visual.
	//Ich schätze mal, dass alle benutzten Visuals hier gesammelt werden
	//und Vobs nur Kopien der Referenzen halten.
	zCArray   <zCVisual*>     visualList;
};

oCGame* ogame;
