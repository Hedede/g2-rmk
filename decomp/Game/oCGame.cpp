//statics:
const int GAME_VIEW_MAX = 6;

class oCGame : public zCSession {
public:
	virtual ~oCGame();
	virtual void HandleEvent(int);
	virtual void Init();
	virtual void Done();
	virtual void Render();
	virtual void RenderBlit();
	virtual void SaveWorld(zSTRING const &,zCWorld::zTWorldSaveMode,int,int);
	virtual void LoadWorld(zSTRING const &,zCWorld::zTWorldLoadMode);
	virtual void SetTime(int,int,int);
	virtual void GetTime(int &,int &,int &);
	virtual void DesktopInit();
	virtual void EnterWorld(oCNpc *,int,zSTRING const &);
	virtual void Pause(int);
	virtual void Unpause();
	virtual void SetDrawWaynet(int);
	virtual void GetDrawWaynet();
	virtual void RenderWaynet();
	virtual void EnvironmentInit();
	virtual void SetRanges();
	virtual void SetRangesByCommandLine();
	virtual void GetStartPos();
	virtual void SetGameInfo(oCGameInfo *);
	virtual void LoadParserFile(zSTRING const &);
	virtual void TriggerChangeLevel(zSTRING const &,zSTRING const &);
	virtual void GetGameWorld();
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
	virtual void NpcInit(zCTree<zCVob> *);
	virtual void NpcInit();
	virtual void SetAsPlayer(zSTRING const &);

	oCNewsManager& GetNewsManager()
	{
		return this->newsman;
	}

	oCInfoManager& GetInfoManager()
	{
		return infoman;
	}

	oCGuilds& GetGuilds()
	{
		return guilds;
	}
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
	}

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

	int debugChannels;          //int
	int debugAllInstances;      //zBOOL

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
		if ( _defaultfactormotion != 0.0 && player->IsUnconscious() )
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
			zVEC3 pos;
			pos[0] = player->trafoObjToWorld.m[0][3];
			pos[1] = player->trafoObjToWorld.m[1][3];
			pos[2] = player->trafoObjToWorld.m[2][3];
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
