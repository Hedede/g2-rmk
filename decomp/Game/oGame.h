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
	zREAL  cliprange;
	zREAL  fogrange;
	zBOOL  inScriptStartup;
	zBOOL  inLoadSaveGame;
	zBOOL  inLevelChange;

	enum oTGameDialogView
	{
		GAME_VIEW_SCREEN        ,
		GAME_VIEW_CONVERSATION  ,
		GAME_VIEW_AMBIENT       ,
		GAME_VIEW_CINEMA        ,
		GAME_VIEW_CHOICE        ,
		GAME_VIEW_NOISE         ,
		GAME_VIEW_MAX
	};

	//Views sind Kanäle, über die die Engine
	//Informationen anzeigen kann.
	//Beispiel sind die normalen Dialoguntertitelboxen.
	zCView*  array_view[GAME_VIEW_MAX];
	zBOOL    array_view_visible[GAME_VIEW_MAX];
	zBOOL    array_view_enabled[GAME_VIEW_MAX];

	oCSavegameManager*  savegameManager;
	zCView*             game_text;
	zCView*             load_screen;
	zCView*             save_screen;
	zCView*             pause_screen;
	oCViewStatusBar*    hpBar;
	oCViewStatusBar*    swimBar;
	oCViewStatusBar*    manaBar;
	oCViewStatusBar*    focusBar;
	zBOOL               showPlayerStatus;

	// Debugsachen
	zBOOL  game_drawall;           //  //"toggle Desktop"
	zBOOL  game_frameinfo;         //
	zBOOL  game_showaniinfo;       //
	zBOOL  game_showwaynet;        //
	zBOOL  game_testmode;          //
	zBOOL  game_editwaynet;        //
	zBOOL  game_showtime;          //
	zBOOL  game_showranges;        //
	zBOOL  drawWayBoxes;           //
	zBOOL  scriptStartup;          //
	zBOOL  showFreePoints;         //
	oCNpc* showRoutineNpc;         //

	// Levelinfos
	zBOOL loadNextLevel;          //
	zSTRING  loadNextLevelName;   //
	zSTRING  loadNextLevelStart;  //

	// Spielerspezifika
	zVEC3 startpos;            //
	oCGameInfo* gameInfo;               // //etwas auf den ersten Blick uninteressantes
	zCVobLight*  pl_light;               //
	zREAL        pl_lightval;            //

	// Timer
	oCWorldTimer* wldTimer;               //
	zREAL timeStep;               //                          // Ermöglicht Einzelbildschaltung
	zBOOL singleStep;             //

	// Referenzen auf Einzelstückklassen.
	oCGuilds*            guilds;                 //
	oCInfoManager*       infoman;                //
	oCNewsManager*       newsman;                //
	oCSVMManager*        svmman;                 //
	oCTradeManager*      trademan;               //
	oCPortalRoomManager* portalman;              //
	oCSpawnManager*      spawnman;               //

	//Zeug
	zREAL  music_delay;            //
	oCNpc* watchnpc;               //

	//Kurz nachdem Laden kurze Pause, damit
	//sich die Systemressourcen erholen.
	zBOOL  m_bWorldEntered;        //
	zREAL  m_fEnterWorldTimer;     //

	//Klar:
	int initial_hour;           //int
	int initial_minute;         //int

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

	int oldRoutineDay;          //int

	zCListSort<TObjectRoutine>    objRoutineList;

	zCListSort<TObjectRoutine>* currentObjectRoutine;

	//ProgressBar
	zCViewProgressBar* progressBar;                            //

	//Nicht jedes Fass in der Welt hat ein privates Visual.
	//Ich schätze mal, dass alle benutzten Visuals hier gesammelt werden
	//und Vobs nur Kopien der Referenzen halten.

	zCArray   <zCVisual*>     visualList;
};

oCGame* ogame;
