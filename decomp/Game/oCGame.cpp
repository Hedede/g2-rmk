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

void oCGame::SetShowPlayerStatus(int show)
{
	if ( !show ) {
		screen->RemoveItem(hpBar);
		screen->RemoveItem(swimBar);
		screen->RemoveItem(manaBar);
		screen->RemoveItem(focusBar);
	}
	showPlayerStatus = show;
}

void oCGame::SetGameInfo(oCGameInfo* gameinfo)
{
	if (this->gameInfo != gameinfo)
		Delete(this->gameInfo);

	if (!gameinfo) {
		gameinfo = zfactory->CreateGameInfo();
		gameinfo->Init();
	}

	gameInfo = gameinfo;
}

void oCGame::SetShowNews(oCNpc* npc)
{
	for (auto list : GetGameWorld()->voblist_npcs )
		list->SetShowNews(npc == list);
}

oCWorld* oCGame::GetGameWorld() const
{
	return zDYNAMIC_CAST<oCWorld>(GetWorld());
}

int oCGame::GetHeroStatus() const
{
	if (oCNpc::player) {
		auto ai = zDYNAMIC_CAST<oCAIHuman>(oCNpc::player->callback_ai);
		if (ai)
			return ai->GetEnemyThreat();
	}
	return 0;
}

void oCGame::SetCameraPosition()
{
	if (GetCameraVob() && GetWorld()) {
		GetCameraVob()->SetCollDetStat(0);
		GetCameraVob()->SetCollDetDyn(0);

		GetWorld()->RemoveVob(GetCameraVob());
		GetWorld()->AddVob(GetCameraVob());

		GetCameraVob()->SetAI(GetCameraAI());
		GetCameraVob()->SetSleeping(0);
	}
}

void oCGame::SetRanges()
{
	GetCamera()->SetFarClipZ(cliprange);
	zrenderer->SetFogRange(fogrange, GetCamera()->farClipZ, 0);
	zrenderer->SetFog(1);
}

void oCGame::RenderBlit()
{
	zCSession::RenderBlit();
}

void oCGame::RenderWaynet()
{
	if ( game_showwaynet ) {
		auto waynet = GetWorld()->wayNet;
		if (waynet) {
			waynet->Draw(GetCamera());
		}
	}
}

int oCGame::OpenView(oEGameDialogView view)
{
	if ( !array_view_visible[view] && array_view_enabled[view] ) {
		array_view_visible[view] = 1;
		array_view[view]->Open();
	}
	return 1;
}

int oCGame::CloseView(oEGameDialogView view)
{
	if ( !array_view_visible[view] && array_view_enabled[view] ) {
		array_view_visible[view] = 0;
		array_view[view]->Close();
	}
	return 1;
}

int oCGame::LoadWorld(zSTRING const& pwf, zCWorld::zTWorldLoadMode mode)
{
	int ret = 0;
	if ( GetWorld() ) {
		ret = GetWorld()->LoadWorld(pwf, mode);
		oCGame::RemoveHelperVobs(GetWorld()->globalVobTree.firstChild);
	}
	return ret;
}

void oCGame::NpcInit()
{
	NpcInit(GetWorld()->globalVobTree);

	rtnMan.InitWayBoxes();
	/*
	for (auto npc : ogame->GetGameWorld()->voblist_npcs) {
		if ( !npc->IsDead() || !npc->IsAPlayer() )
			rtnMan->CreateWayBoxes(npc);
	}*/
}

void oCGame::NpcInit(zCTree<zCVob>* vobtree)
{
	zCVob* vob = vobtree->data;
	if ( vob && vob->type == VOB_TYPE_NPC )
		vob->SetSleepingMode(1);

	for (auto i = vobtree->firstChild; i; i = i->next )
		NpcInit(i);
}

void oCGame::InitNpcAttitudes(oCGame* this)
{
	if (!oCNpc::player)
		return;

	for (auto npc : GetGameWorld()->voblist_npcs) {
		if (npc == oCNpc::player)
			continue;

		// may be inlined
		auto playerGuild = oCNpc::player->GetTrueGuild();
		auto npcGuild = npc->GetTrueGuild();

		auto att = ogame->guilds->GetAttitude(npcGuild, playerGuild);

		npc->SetTmpAttitude(att);
		npc->SetAttitude(att);
	}
}

void oCGame::RefreshNpcs()
{
	for (auto npc : GetGameWorld()->voblist_npcs) {
		if ( !npc->IsAPlayer() )
			npc->RefreshNpc();
	}
	DeleteTorches();
}

void oCGame::CloseSavescreen()
{
	Delete(progressBar);
	Delete(save_screen);
	if ( GetWorld() )
		GetWorld()->progressBar = 0;
}

void oCGame::CloseLoadscreen()
{
	Delete(progressBar);
	Delete(load_screen);
	if ( GetWorld() )
		GetWorld()->progressBar = 0;
}

void oCGame::UpdateViewSettings()
{
	bool animatedWindows = zoptions->ReadBool(zOPT_SEC_GAME, "animatedWindows", 1);

	array_view[0]->fxClose = 0;
	array_view[0]->fxOpen = 0;

	array_view[1]->fxClose = animatedWindows;
	array_view[1]->fxOpen = animatedWindows;

	array_view[2]->fxClose = 0;
	array_view[2]->fxOpen = 0;

	array_view[3]->fxClose = 0;
	array_view[3]->fxOpen = 0;

	array_view[4]->fxClose = animatedWindows;
	array_view[4]->fxOpen = animatedWindows;

	array_view[5]->fxClose = animatedWindows;
	array_view[5]->fxOpen = animatedWindows;
}

void oCGame::ClearObjectRoutineList()
{
	objRoutineList.DeleteListDatas();
	currentObjectRoutine = 0;
}

void oCGame::CheckObjectRoutines()
{
	if (!currentObjectRoutine) {
		if ( oldRoutineDay != wldTimer->GetDay() ) {
			currentObjectRoutine = objRoutineList.next;
			oldRoutineDay = wldTimer->GetDay();
		}
	}
	while (currentObjectRoutine) {
		auto rtn = currentObjectRoutine->Get();

		if (!wldTimer->IsLater(rtn->hour, rtn->min))
			break;

		if (rtn->type == 0)
			oCMobInter::SetAllMobsToState(GetGameWorld(), rtn->objectName, rtn->state);

		if (rtn->type == 1)
			auto vob = GetGameWorld()->SearchVobByName(rtn->objectName);
			if ( vob ) {
				if (rtn->state == 0) {
					vob->GetEM()->OnUnrigger(0,0);
				if (rtn->state == 1) {
					vob->GetEM()->OnTrigger(0,0);
			}
		}

		currentObjectRoutine = currentObjectRoutine->next;
	}
}

void oCGame::RemoveHelperVobs(zCTree<zCVob>* node)
{
	for (auto i = node; i; i = i->next ) {
		switch ( i->data->type ) {
		case VOB_TYPE_LIGHT:
			i->data->SetVisual(0);
			i->data->flags.showVisual = 1; // (flags & 0xFE) ^ 1
			break;
		case VOB_TYPE_WAYPOINT:
		case VOB_TYPE_SOUND:
		case 4:
		case 5:
		case VOB_TYPE_STARTPOINT:
			i->data->flags.showVisual = 0;
			i->data->SetCollDetDyn(0);
			i->data->SetCollDetStat(0);
		default:
			break;
		}

		RemoveHelperVobs(i->firstChild);
	}
}

void oCGame::SetTime(int day, int hour2, int min2)
{
	int hour1, min1;

	wldTimer->GetTime(hour1, min1);
	wldTimer->SetDay(day + hour2 / 24);
	wldTimer->SetTime(hour2, min2);

	rtnMan.SetDailyRoutinePos(1);
	SetObjectRoutineTimeChange(hour1, min1, hour2, min2);

	if ( spawnman )
		spawnman->SpawnImmediately(1);
}

void oCGame::RecurseCacheVobs(zCTree<zCVob>* tree)
{
	if ( tree ) {
		auto vob = tree->data;
		if (vob && vob->visual)
			visualList.Insert(vob->visual);

		for (auto i = tree->firstChild; i; i = i->next )
			RecurseCacheVobs(i);
	}
}

void oCGame::CacheVisualsOut()
{
	for (auto visual : visualList)
		Release(visual);

	visualList.Clear();
}

void oCGame::CacheVisualsIn()
{
	if ( GetWorld() ) {
		visualList.Clear();

		RecurseCacheVobs(GetWorld()->globalVobTree);

		zINFO(5, "U: Cached "_s + Count(visualList) + " visuals."); // 2109
	}
}

int oCGame::HandleEvent(int key)
{
	zCWorld* world = GetWorld();
	oCNpc* player = oCNpc::player;
	if ( world->csPlayer->GetPlayingGlobalCutscene() || !player )
		return 0;

	switch ( zinput->GetFirstBindedLogicalKey(key) ) {
	case GAMEKEY_LOCK_TARGET:
		if ( !oCGame::s_bUseOldControls )
			oCNpc::s_bTargetLocked = oCNpc::s_bTargetLocked == 0;
		return 1;
	case GAMEKEY_SCREEN_STATUS:
		if ( player->inventory.IsOpen() )
			player->inventory.Close();
		player->OpenScreen_Status();
		return 1;
	case GAMEKEY_SCREEN_LOG:
		if ( player->inventory.IsOpen() )
			player->inventoryy.Close();
		oCNpc::OpenScreen_Log();
		return 1;
	case GAMEKEY_SCREEN_MAP:
		zINFO(8,"N: MAP: GAME_SCREEN_MAP"); // 630

		if ( !player )
			break;

		if ( player->IsUnconscious() )
			break;
		if ( player->GetInteractMob() )
			break;
		if ( !player->GetEM()->IsEmpty(1) )
			break;

		if (player->GetAnictrl()->IsStanding() &&
		    player->GetBodyState() != 5        &&
		    player->GetBodyState() != 7)
		{
			int sym = zparser.GetIndex("PLAYER_HOTKEY_SCREEN_MAP");
			if (!sym) {
				zFAULT("N: MAP: Skript function PLAYER_HOTKEY_SCREEN_MAP not found!"); // 643
				break;
			}

			if ( ogame && ogame->GetGameWorld() )
			{
				int ret = *zparser->CallFunc(sym);
				player->ClearEM();
				if ( player->inventory2.IsOpen())
					player->inventory2.Close();
				player->OpenScreen_Map(ret);
			}
		}
		return 1;
	case GAMEKEY_INVENTORY:
		if (ztimer.frameTimeFloat != 0.0 && player->IsUnconscious() )
			break;

		if ( !player )
			break;

		if ( !player->GetEM()->IsEmpty(1) )
			break;

		if ( player->GetInteractMob() )
			break;

		player->OpenInventory(0);
		player::GetAnictrl()->StopLookAtTarget();

		return 1;
	default:
		break;
	}

	switch ( key ) {
	case KEY_F1:
	case KEY_F2:
	case KEY_F3:
	case KEY_F4:
		if ( !game_testmode ) {
			world = ogame->GetWorld();
			zCSkyControler* sktrl = world->GetActiveSkyControler();
			if ( sktrl ) {
				float dist = 1.0;
				switch ( key ) {
				case KEY_F1:
					dist = zoptions->ReadReal("GAME", "SHORTKEY1FARPLANEDIST", 0.80);
					break;
				case KEY_F2:
					dist = zoptions->ReadReal("GAME", "SHORTKEY2FARPLANEDIST", 1.20);
					break;
				case KEY_F3:
					dist = zoptions->ReadReal("GAME", "SHORTKEY3FARPLANEDIST", 2.00);
					break;
				case KEY_F4:
					dist = zoptions->ReadReal("GAME", "SHORTKEY4FARPLANEDIST", 3.00);
					break;
				default:
					break;
				}

				if ( dist < 0.2 )
					dist = 0.2;
				if ( dist > 3.0 )
					dist = 3.0;

				int steps = int(dist * 100.0) / 20 - 1;
				if (steps < 0)
					steps = 0
				if (steps >= 14)
					steps = 14;

				zoptions->WriteInt(zOPT_SEC_PERFORMANCE, "sightValue", steps, 1);
				sktrl->SetFarZScalability(dist);
			}
			return 1;
		}

		return TestKeys(key);
	case KEY_1:
		if ( !player->CanDrawWeapon() )
			break;

		if ( player->GetSpellBook() )
			player->GetSpellBook()->Close(1);

		if (!zinput->KeyToggled(KEY_1))
			return 1;

		if ( player->inventory2.IsOpen())
			player->inventory2.Close();

		auto* mob = dynamic_cast<oCMobContainer*>(player->GetInteractMob());
		if (mob) {
			mob->Close(player);
			mob->Reset();
		}


		oCMsgWeapon* msg;
		if ( player->GetWeaponMode() <= FMODE_MAGIC ) {
			if ( player->GetWeaponMode() != FMODE_NONE ) {
				msg = new oCMsgWeapon(3, 0, 0);
				player->GetEM()->OnMessage(msg, player);
			}
			msg = new oCMsgWeapon(0, FMODE_FIST, 0);
			player->GetEM()->OnMessage(msg, player);
		}
		return 1;
	case KEY_2:
		if ( !player->CanDrawWeapon() || !zinput->KeyToggled(KEY_2))
			return 1;

		if ( player->GetSpellBook() )
			player->GetSpellBook()->Close(1);

		if ( player->inventory2.IsOpen())
			player->inventory2.Close();

		auto* mob = dynamic_cast<oCMobContainer*>(player->GetInteractMob());
		if (mob) {
			mob->Close(player);
			mob->Reset();
		}

		auto weapon = player->GetEquippedRangedWeapon();
		if (player->GetWeaponMode() == FMODE_NONE)
		{
			if ( !weapon || !player->IsMunitionAvailable(weapon) )
				return 1;

			msg = new oCMsgWeapon(0, FMODE_FAR, 0);
			player->GetEM()->OnMessage(msg, player);
			break;
		}

		if ( player->GetWeaponMode() == FMODE_FAR || oCNpc::GetWeaponMode(player) == 6 )
		{
			msg = new oCMsgWeapon(3, 0, 0);
			player->GetEM()->OnMessage(msg, player);
			break;
		}

		if ( weapon && player->IsMunitionAvailable(weapon) )
		{
			msg = new oCMsgWeapon(3, 0, 0);
			player->GetEM()->OnMessage(msg, player);
			msg = new oCMsgWeapon(0, FMODE_FAR, 0);
			player->GetEM()->OnMessage(msg, player);
		}
		return 1;
	case KEY_3:
		if ( !player->CanDrawWeapon() || !zinput->KeyToggled(KEY_3))
			return 1;

		if ( player->inventory2.IsOpen())
			player->inventory2.Close();


		auto* mob = dynamic_cast<oCMobContainer*>(player->GetInteractMob());
		if (mob) {
			mob->Close(player);
			mob->Reset();
		}
		/*
		auto* mob = dynamic_cast<oCMobContainer*>(player->GetInteractMob());
		if (mob) {
			mob->Close(player);
			mob->Reset();
		}*/

		if ( player->GetWeaponMode() )
		{
			oCMsgWeapon* msg = new oCMsgWeapon(3, 0, 0);
			player->GetEM()->OnMessage(msg, player);

			if ( player->GetWeaponMode() >= FMODE_MAGIC )
				break;
		}

		msg = new oCMsgWeapon(0, FMODE_MAGIC, 0);
		player->GetEM()->OnMessage(msg, player);
		break;
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
	case KEY_0:
		if (player->CanDrawWeapon()) {
			if (player->inventory2.IsOpen())
				break;

			auto* mob = dynamic_cast<oCMobContainer*>(player->GetInteractMob());
			if (mob) {
				mob->Close(player);
				mob->Reset();
			}

			if (!player->HasMagic())
				break;

			char keyChar = GetCharFromDIK(key, 0, 0);
			if (keyChar == '0' )
				keyChar = ':';

			oCMag_Book* mbook = player->GetSpellBook();
			if ( player->GetWeaponMode() == FMODE_MAGIC)
			{
				oCSpell* spell = mbook->GetSelectedSpell();
				if ( spell && spell->spellSlot == (keyChar - '3')) {
					oCMsgWeapon* msg = new oCMsgWeapon(3, 0, 0);
					player->GetEM()->OnMessage(msg, player);
					break;
				}
			}

			if ( player->GetAnictrl()->actionMode != 5)
			{
				int spellSlot = mbook->GetNoOfSpellByKey(keyChar - '3');
				if ( spellSlot )
				{
					mbook->SetFrontSpell(spellSlot - 1);
					oCSpell* spell = mbook->GetSelectedSpell();
					if ( spell ) {
						zSTRING spellName = spell->GetName();
						zCView::stdoutput->PrintCXY(spellName);
						zCView::stdoutput->PrintTimedCXY(spellName, 1000.0, 0);
					}
					if ( !player->GetSpellBook()->_wtf_unk_prolly_IsOpen )
					{
						if ( player->GetWeaponMode() < FMODE_MAGIC )
						{
							if ( player->GetWeaponMode() != 0 )
							{
								msg = new oCMsgWeapon(3, 0, 0);
								player->GetEM()->OnMessage(msg, player);
							}
							msg = new oCMsgWeapon(0, FMODE_MAGIC, 0);
							player->GetEM()->OnMessage(msg, player);
						}
					}
				}
			}
		}
		break;
	case KEY_F8:
		if ( game_testmode )
			return TestKeys(key);

		if (zinput->KeyPressed(KEY_LMENU) &&
		    zinput->KeyPressed(KEY_LCONTROL) && player)
		{
			zVEC3 pos = player->GetPositionWorld();
			player->EmergencyResetPos(pos);
		}
		break;
	default:
		if ( !game_testmode )
			break;

		return TestKeys(key);
	}
	return 1;
}

void oCGame::DeleteNpcs(oCNpc* npc1, oCNpc* npc2, float radius) // unused
{
	for (auto npc : GetGameWorld()->voblist_npcs) {
		if (npc != npc1 && npc2 && npc != npc2) {
			if (npc2->GetDistanceToVobApprox(npc) > radius) {
				npc->states.hasRoutine = 0;
				GetGameWorld()->RemoveVob(npc);
			}
		}
	}
}

void oCGame::DeleteTorches()
{
	for (auto vob : GetGameWorld()->voblist) {
		auto item = zDYNAMIC_CAST<oCItem>(vob);
		if ( item && item->homeWorld ) {
			if (item->GetInstanceName == "ITLSTORCHBURNING") {
				auto ai = zDYNAMIC_CAST<oCAIVobMove>(item->callback_ai);
				if (ai) {
					item->SetAI(0);
					item->homeWorld->RemoveVobSubtree(item);
				}
			}
		}
	}
}

void oCGame::Unpause()
{
	if (singleStep) {
		zINFO(5,"B: GAM: Game unpaused"); // 2767,

		Delete(pause_screen);

		screen->Render();

		singleStep = 0;
		timeStep = -1.0;
	}
}

int oCGame::IsInWorld(zSTRING& levelpath)
{
	if ( levelpath.Length() > 0) {
		zFILE_FILE file{levelpath};
		auto filename = file.GetFilename();
		auto worldname = GetGameWorld()->GetWorldName();
		return filename == worldname;
	}

	return GetGameWorld()->GetWorldName().IsEmpty();
}

void oCGame::InsertObjectRoutine(int type, zSTRING const& name, int hour1, int min1, int state)
{
	auto rtn = new TObjectRoutine();

	rtn->name = name;
	rtn->hour = hour1;
	rtn->min = min1;
	rtn->state = state;
	rtn->type = type;

	objRoutineList.Insert(rtn);
}

void oCGame::CompileWorld()
{
	if (GetWorld()->compiled)
		return;

	bool outdoor = zoptions->Parm("OUTDOOR");
	zTBspTreeMode mode = outdoor;

	auto gc = zoptions->ReadString(zOPT_SEC_INTERNAL, "gameCompile", "1.0");
	auto gameCompile = gc.ToFloat();

	GetWorld()->CompileWorld(mode, gameCompile, 0, 0, 0);

	zCWorld::zTStaticWorldLightMode light = 0;
	GetWorld()->GenerateStaticWorldLighting(light, 0);
}

int oCGame::LoadParserFile(zSTRING const& fileName)
{
	zSTRING scriptfile = fileName;
	scriptfile.Upper();

	zparser.Reset();
	DefineExternals_Ulfi(zparser);

	zparser.EnableTreeLoad(0);
	zparser.EnableTreeSave(0);

	zparser.Parse(scriptfile);

	zparser.AddClassOffset("C_NPC", 288);
	zparser.AddClassOffset("C_ITEM", 288);

	zparser.CreatePCode();

	return !zparser.Error();
}

void oCGame::UpdatePlayerStatus()
{
	if (!oCNpc::player)     return;
	if (!showPlayerStatus ) return;

	screen->RemoveItem(hpBar);
	screen->RemoveItem(swimBar);
	screen->RemoveItem(manaBar);
	screen->RemoveItem(focusBar);

	auto world = GetWorld();
	if (world && world->csPlayer->GetPlayingGlobalCutscene())
		return;

	auto player = oCNpc::player;
	auto& inventory = player->inventory;

	int change_hp = 0;
	int change_mana = 0;
	if ( inventory->IsOpen() ) {
		auto item = inventory->GetSelectedItem();
		if ( item ) {
			if ( !item->nutrition ) {
				for (auto i : {0,1,2}) {
					if (item->change_atr[i] == ATR_HITPOINTS)
						change_hp += item->change_value[i];
				}
			}
			for (auto i : {0,1,2}) {
				if (item->change_atr[i] == ATR_MANA)
					change_mana += item->change_value[i];
			}
		}
	}

	int hp = player->GetAttribute(ATR_HITPOINTS);
	int hp_max = player->GetAttribute(ATR_HITPOINTS_MAX);
	screen->InsertItem(hpBar);
	hpBar->SetMaxRange(0.0, hp_max);
	hpBar->SetRange(0.0, hp_max);
	hpBar->SetPreview(hp + change_hp);
	hpBar->SetValue(hp);

	auto anictrl = oCNpc::player->GetAnictrl();
	if (anictrl && anictrl->walkmode == 5) {
		float swimTime, diveTime, diveCtr;
		player->GetSwimDiveTime(swimTime, diveTime, diveCtr);
		if (diveTime == -1000000.0)
			diveCtr = diveTime;

		screen->InsertItem(swimBar, 0);
		swimBar->SetMaxRange(0.0, diveTime);
		swimBar->SetRange(0.0, diveTime);
		swimBar->SetValue(diveCtr);
	}

	if ( inventory->IsOpen() || player->GetWeaponMode() == 7 ) {
		int mana = player->GetAttribute(ATR_MANA);
		int mana_max = player->GetAttribute(ATR_MANA_MAX);
		if ( mana_max > 0 ) {
			screen->InsertItem(manaBar, 0);
			manaBar->SetMaxRange(0.0, mana_max);
			manaBar->SetRange(0.0, mana_max);
			manaBar->SetPreview(mana + change_mana);
			manaBar->SetValue(mana);

		}
	}

	if ( oCGame::load_focus_options ) {
		oCGame::load_focus_options = 0;
		if ( zgameoptions ) {
			game_show_focus = zgameoptions->ReadBool(zOPT_SEC_OPTIONS, "show_Focus", game_show_focus);
			game_show_focusitem = zgameoptions->ReadBool(zOPT_SEC_OPTIONS, "show_FocusItm", game_show_focusitem);
			game_show_focusmob = zgameoptions->ReadBool(zOPT_SEC_OPTIONS, "show_FocusMob", game_show_focusmob);
			game_show_focusnpc = zgameoptions->ReadBool(zOPT_SEC_OPTIONS, "show_FocusNpc", game_show_focusnpc);
			game_show_focusbar = zgameoptions->ReadBool(zOPT_SEC_OPTIONS, "show_FocusBar", game_show_focusbar);
		}
	}

	if (!game_show_focus )
		return;

	auto focus = player->GetFocusVob();
	if (!focus)
		return;

	oCMob* mob;
	oCNpc* npc;
	oCItem* item;

	if (mob = zDYNAMIC_CAST<oCMob>(focus)) {
		if (mob = zDYNAMIC_CAST<oCMob>(focus);!game_show_focusmob)
			retmob = zDYNAMIC_CAST<oCMob>(focus);urn;

		name = mob->GetName();
	} else if (npc = zDYNAMIC_CAST<oCNpc>(focus)) {
		if (!game_show_focusitem)
			return;

		name = item->GetName(1);
	} else if(item = dynamic_cast<oCItem>(focus)) {
		if (npc->noFocus)
			return;

		if (game_show_focusbar) {
			int hp = npc->GetAttribute(ATR_HITPOINTS);
			int hp_max = npc->GetAttribute(ATR_HITPOINTS_MAX);
			if (hp_max > 0) {
				screen->InsertItem(focusBar);
				focusBar->SetMaxRange(0.0, hp_max);
				focusBar->SetRange(0.0, hp_max);
				focusBar->SetValue(hp);
			}
		}

		if (!game_show_focusnpc)
			return;

		name = npc->GetName(0);
	}

	auto visual = focus->visual;
	if (!visual)
		return;
 
	zVEC3 pos;

	auto vfx = zDYNAMIC_CAST<oCVisualFX>(vob->visual);
	if (vfx) {
		pos = vob->trafoObjToWorld->GetTranslation();
		pos[1] += vfx->nextOnTimer * 1.6;
		float max = focus->bbox3D.maxs[1] + 100.0;
		if (pos[1] > max)
			pos[1] = max;
	} else {
		pos = (focus->bbox3D.mins + focus->bbox3D.maxs) * 0.5;
		pos[1] += (focus->bbox3D.maxs - focus->bbox3D.mins) * 0.82;
	}

	zCCamera::activeCam->Activate();
	auto projPoint = zCCamera::activeCam->camMatrix * pos;
	if ( projPoint.Z() <= 0.0 )
		return;

	float invZ = 1.0 / projPoint.Z();

	float x = zCCamera::activeCam->xcenter + projPoint.X() * zCCamera::activeCam->viewDistanceX * invZ;
	float y = zCCamera::activeCam->ycenter - projPoint.Y() * zCCamera::activeCam->viewDistanceY * invZ;

	x -= screen->nax(screen->FontSize(name) / 2);

	auto anx = screen->anx(x);
	auto any = screen->any(y);

	if ( any < screen->FontY() )
		any = screen->FontY();
	if ( any > 0x2000 - screen->FontY() )
		any = 0x2000 - screen->FontY();

	if ( anx >= 0 ) {
		if ( anx > 0x1FFF - screen->FontSize(name) )
			anx = 0x1FFF - screen->FontSize(name);
	} else {
		anx = 0;
	}

	if ( oCNpc::s_bTargetLocked )
		name += " (locked)";

	screen->Print(anx, any, name + "\n");
}

void oCGame::SwitchCamToNextNpc()
{
	bool showAI = 0;
	if ( watchn ) {
		showAI = watchn->callback_ai->GetShowAI();
		watchn->callback_ai->SetShowAI(0);
	}


	do {
		watchn = nullptr;

		int i;
		for (auto npc : GetGameWorld()->voblist_npcs ) {
			if (i == npcNr) {
				watchn = npc;
				break;
			}
			++i;
		}

		if ( watchn ) {
			if ( watchn->homeWorld ) {
				watchn->callback_ai->SetShowAI(showAI);
			} else {
				watchn = 0;
			}
		} else {
			npcNr = -1;
		}

		++npcNr;
	} while ( !watchn );

	GetCameraAI()->SetTarget(watchn);
	if ( watchn && (watchn->variousFlags & 4) == 4)
		GetCameraAI()->__ghostAlpha = zoptions->ReadReal("INTERNAL", "GhostAlpha", 0.3);

	GetCameraAI()->ReceiveMsg(0x8000);

	auto tmp = oCNpc::player;
	if ( oCNpc::player->flags2.sleepingMode )
	{
		HandleEvent(KEY_F4);
	}
	else
	{
		oCNpc::player = 0;
		HandleEvent(v1, KEY_F4);
		oCNpc::player = tmp;
	}
}

void oCGame::TriggerChangeLevel(zSTRING const& levelpath, zSTRING const& start)
{
	loadNextLevel = 1;
	loadNextLevelName = levelpath;
	loadNextLevelStart = start;
	if ( oCNpc::player ) {
		if ( oCNpc::player->GetWeaponMode() == FMODE_MAGIC )
			oCNpc::player->EV_ForceRemoveWeapon(nullptr);
	}
}

void oCGame::ChangeLevel(zSTRING const& levelpath, zSTRING const& start)
{
	zparser.ClearAllInstanceRefs();
	if ( zresMan ) {
		zresMan->PurgeCaches(0);
		zresMan->cacheInImmedMsec = 1.0e8;
	}

	OpenLoadscreen(0, levelpath);
	bool musicEnabled = zoptions->ReadBool(zOPT_SEC_SOUND, "musicEnabled", 1);

	zCZoneMusic::SetAutochange(0);
	zCMusicSystem::DisableMusicSystem(1);

	if ( zsound ) {
		zsound->StopAllSounds();
		zFILE_FILE file{levelpath};
		auto filename = "LOADING_SFX_" + file->GetFilename();
		auto sfx = zsound->LoadSoundFXScript(filename.Upper());
		if ( sfx ) {
			zsound->PlaySound(sfx, 0);
			Release(sfx);
		}
	}

	if ( progressBar )
		progressBar->SetPercent(0, "");

	auto player = oCNpc::player;
	if ( oCNpc::player ) {
		oCNpc::player->AddRef();
		oCNpc::player->groundPoly = 0;
		oCNpc::player->ClearPerceptionLists();
		GetGameWorld()->RemoveVob(oCNpc::player);
		oCNpc::player = 0;
	}

	if ( progressBar )
		progressBar->SetPercent(2, "");

	oCNpc::dontArchiveThisNpc = player;

	DeleteTorches();

	inLevelChange = 1;

	if ( progressBar )
		progressBar->SetRange(5, 35);

	WriteSavegame(SAVEGAME_SLOT_CURRENT, 0);

	if ( progressBar )
		progressBar->ResetRange();

	inLevelChange = 0;

	oCNpc::dontArchiveThisNpc = 0;

	int slot;
	if ( CheckIfSavegameExists(levelpath) ) {
		zINFO(9,"B: (oCGame::ChangeLevel) Savegame exists"); // 2402,
		slot = SAVEGAME_SLOT_CURRENT;
	} else {
		zINFO(9,"B: (oCGame::ChangeLevel) Savegame does not exist"); // 2402,
		slot = SAVEGAME_SLOT_NEW;
	}

	if ( progressBar )
		progressBar->SetPercent(37, "");

	spawnman->ClearList();
	GetGameWorld()->DisposeWorld();
	CamInit();
	oCVob::ClearDebugList();

	if ( portalman )
		portalman->CleanUp();

	objRoutineList.DeleteDatas();

	currentObjectRoutine = 0;

	CheckObjectConsistency(0);

	if ( progressBar )
		progressBar->SetPercent(40, "");

	if ( progressBar )
		progressBar->SetRange(40, 92);

	LoadWorld(slot, levelpath);

	if ( progressBar )
		progressBar->ResetRange();

	if ( progressBar )
		progressBar->SetRange(95, 98);

	if ( player && oCNpc::player )
		zFAULT("U: CHL: Two Players found..."); //2450

	EnterWorld(player, 1, start);

	if ( progressBar )
		progressBar->ResetRange();

	CallScriptInit();

	Release(player);

	RefreshNpcs();

	loadNextLevel = 0;
	rtnMan.SetDailyRoutinePos(slot != SAVEGAME_SLOT_NEW);

	int day, hour, min;
	GetTime(day, hour, min);

	SetObjectRoutineTimeChange(0, 0, hour, min);

	if ( spawnman )
		spawnman->SpawnImmediately(1);

	if ( slot == SAVEGAME_SLOT_NEW )
		InitNpcAttitudes();

	ogame->infoman->RestoreParserInstances();

	if (progressBar)
		progressBar->SetPercent(100, "");

	zCZoneMusic::SetAutochange(musicEnabled);
	zCMusicSystem::DisableMusicSystem(!musicEnabled);

	CloseLoadscreen();

	if ( zresMan )
		zresMan->cacheInImmedMsec = 1500.0;
}

void oCGame::EnterWorld(oCNpc* playerVob, int changePlayerPos, zSTRING const& startpoint)
{
	zINFO(9,"B: (oCGame::EnterWorld)"); // 3011,

	if ( progressBar )
		progressBar->SetPercent(0, "");

	if ( progressBar )
		progressBar->SetRange(0, 55);

	if (changePlayerPos) {
		SetupPlayers(playerVob, startpoint);
	} else {
		auto pinfo = dynamic_cast<oCPlayerInfo>(zCPlayerInfo::GetActivePlayer());
		if (!pinfo) {
			pinfo = zfactory->CreatePlayerInfo();
			gameInfo->AddPlayer(pinfo);
			pinfo->SetPlayerVob(playerVob);
			pinfo->SetActive();
		}
	}

	if ( progressBar )
		progressBar->ResetRange();

	CamInit();

	oCNpc::player->SetAsPlayer();
	if ( playerVob && playerVob->GetAnictrl() )
		playerVob->GetAnictrl()->SetFightAnis(playerVob->GetWeaponMode());

	if ( progressBar )
		progressBar->SetPercent(60, "");

	EnvironmentInit();

	if ( progressBar )
		progressBar->SetPercent(75, "");

	NpcInit();

	if ( progressBar )
		progressBar->SetPercent(96, "");

	if ( progressBar )
		progressBar->SetPercent(100, "");

	CloseLoadscreen();

	enterWorldTimer = 0;
	worldEntered = 1;
}
