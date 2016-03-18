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

	//Views sind Kan�le, �ber die die Engine
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
	zREAL timeStep;               //                          // Erm�glicht Einzelbildschaltung
	zBOOL singleStep;             //

	// Referenzen auf Einzelst�ckklassen.
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
	//Ich sch�tze mal, dass alle benutzten Visuals hier gesammelt werden
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

void Game_GotoWP(zSTRING const& wp, zSTRING& msg)
{
	if (oCNpc::player->BeamTo(wp))
		msg = "Player beamed to Waypoint " + wp + ".";
	else
		msg = "Way- or Freepoint not found.";
}

void Game_SaveSCPos()
{
	game_pos = oCNpc::player->trafoObjToWorld.GetTranslation();
}

void Game_LoadSCPos()
{
	oCNpc::player->SetCollDetStat(0);
	oCNpc::player->SetCollDetDyn(0);

	oCNpc::player->SetPositionWorld(game_pos);

	oCNpc::player->SetCollDetStat(1);
	oCNpc::player->SetCollDetDyn(1);
}

void Game_ToggleTestMode(int b)
{
	game_testmode = b;
	if ( ogame )
		ogame->game_testmode = b;
}

void Game_ToggleHoldTime()
{
	game_holdtime = !game_holdtime;
}

void Game_ParserDebug(zSTRING& name)
{
	int index = zparser.GetIndex();
	zparser.ShowCode(index);
}

void Game_ParserDebugOff()
{
	zparser.CloseViews(zparser);
}

void Game_ToggleScreenMode()
{
	zTRnd_ScreenMode mode = zrenderer->Vid_GetScreenMode();
	zrenderer->Vid_SetScreenMode(!mode);
}

void Edit_Changed(zSTRING const& var)
{
	if (var == "ATTRIBUTE")
		oCNpc::player->CheckModelOverlays();
}

int oCGame::ConsoleEvalFunc(zSTRING const& str, zSTRING& msg)
{
	zSTRING word1 = str.PickWord(1," ", zSTR_SKIP).Upper();
	zSTRING word2 = str.PickWord(1," ", zSTR_SKIP).Upper();
	zSTRING word3 = str.PickWord(1," ", zSTR_SKIP).Upper();

	if ( word1 == "INSERT" ) {
		Game_CreateInstance(word2, msg);
		return 1;
	}

	if ( word1 == "GOTO" ) {
		if (word2 == "WAYPOINT") {
			Game_GotoWP(word3, msg);
			return 1;
		}
		if (word2 == "VOB") {
			zCArray<zCVob*> matches; // igrovii spi4ki
			v7 = ogame->GetWorld()->SearchVobListByName(word3, matches);
			if (matches.GetNumInList() <= 0) {
				msg = "No vobs found.", strlen("No vobs found.");
				return 1;
			}

			auto word4 = str.PickWord(4, " ", zSTR_SKIP);
			int index = 1;
			if (!word4.IsEmpty())
				index = word4.ToInt();

			if (index >= 1 && index < matches.GetNumInList()) {
				auto vob = matches[index];

				zVEC3 pos = vob->trafoObjToWorld.GetTranslation();
				if ( pos.x == 0.0 && pos.y == 0.0 && pos.z == 0.0 ) {
					auto npc = zDYNAMIC_CAST<oCNpc>(vob);
					if (npc) {
						if ( !npc->IsSelfPlayer() ) {
							if (npc->states.hasRoutine)
								pos = oCRtnManager::GetRoutinePos(npc);
							else
								pos = npc->states.GetAIStatePosition();
						}
					}
				}


				bool colls = oCNpc::player->GetCollDetStat();
				bool colld = oCNpc::player->GetCollDetDyn();
				oCNpc::player->SetCollDetStat(0);
				oCNpc::player->SetCollDetDyn(0);
				oCNpc::player->SetPositionWorld(pos);
				oCNpc::player->SetCollDetDyn(colld);
				oCNpc::player->SetCollDetStat(colls);
				return 1;
			}

			msg = "Not a valid vob number.";
			return 1;
		}
		return 0;
	}

	if (word1 == "PRINT") {
		if (word2 == "VOB") {
			zCArray<zCVob*> matches;
			ogame->GetWorld()->SearchVobListByName(word3, matches);

			zSTRING msg1 = "Print vob: Found " + matches.GetNumInList() + " vobs.";

			zINFO(1, "S: SC: " + msg1); //3726

			int i = 1;
			for (auto vob : matches) {
				zSTRING print = "S: "_s + (i++) + ": " + vob->GetClassName() + ": " + vob->GetObjectName();
				zINFO(1, print); // 3732, flag:1

				auto item = dynamic_cast<oCItem*>(vob);
				auto npc  = dynamic_cast<oCNpc *>(vob);

				if ( npc ) {
					auto name0 = npc->GetName(0);
					auto name1 = npc->GetName(1);
					print = "S: Name: " + name0 + "/" + name1;
					zINFO(1, print); // 3736
				} else if (item) {
					print = "S: Name: " + item->GetName(0);
					zINFO(1, print); // 3739
				}

				zVEC3 pos = vob->trafoObjToWorld.GetTranslation();
				print = "S: Position: X:"_s + pos.x + " Y:" + pos.y + " Z:" + pos.z;

				if ( vob->homeWorld ) {
					print += "(enabled);
				} else {
					print += "(disabled)";
				}

				zINFO(1,print); // 3751

				zINFO(1,""); //3753
			}
			zINFO(1, ""); //3755 // flag:2
			return 1;
		}
		return 0;
	}
	if (word1 == "DELETE") {
		if (word2 == "PARTICLEFX") {
			if ( pfxc ) {
				pfxc->DeleteLastPFX();
				msg = "ParticleFX deleted";
			}
			return 1;
		}
		return 0;
	}
	if (word1 == "TOGGLE") {
		if (word2 == "HOLDTIME") {
			Game_ToggleHoldTime();
			return 1;
		}
		if (word2 == "DESKTOP")
		{
			ogame->game_drawall = ogame->game_drawall == 0;
			return 1;
		}
		if ( word2 == "DEBUG") {

			zCView::SetShowDebug(!zCView::GetShowDebug());

			if (zCView::GetShowDebug()) {
				msg = "Debug Infos an.";
			} else {
				msg = "Debug Infos aus.";
			}
			return 1;
		}
		if ( word2 == "SPY") {
			game_showerror = !game_showerror;
			if ( game_showerror ) {
				msg = "zSpy-Messages on.";
			} else {
				msg = "zSpy-Messages off.";
			}
			return 1;
		}
		if ( word2 == "TEXT") {
			if ( ogame->game_text->ondesk <= 0 )
				screen->InsertItem(ogame->game_text, 0);
			else
				screen->RemoveItem(ogame->game_text);
			return 1;
		}
		if ( word2 == "WAYNET")
		{
			ogame->game_showwaynet = !ogame->game_showwaynet;
			return 1;
		}
		if ( word2 == "WAYBOXES")
		{
			ogame->drawWayBoxes = !ogame->drawWayBoxes;
			return 1;
		}
		if ( word2 == "FREEPOINTS")
		{
			ogame->showFreePoints = !ogame->showFreePoints;
			return 1;
		}
		if ( word2 == "FRAME")
		{
			ogame->game_frameinfo = !ogame->game_frameinfo;
			return 1;
		}
		if ( word2 == "RANGES")
		{
			ogame->game_showranges = !ogame->game_showranges;
			return 1;
		}
		if ( word2 == "ANIINFO")
		{
			ogame->game_showaniinfo = !ogame->game_showaniinfo;
			return 1;
		}
		if ( word2 == "SCREEN") {
			Game_ToggleScreenMode();
			return 1;
		}
		if ( word2 == "CAMERA")
			goto LABEL_136;
		if ( word2 == "TURNS") {
			v68 = ANI_ENABLE_TURN == 0;
			v69 = -1;
			ANI_ENABLE_TURN = !ANI_ENABLE_TURN;
			if ( ANI_ENABLE_TURN ) {
				msg = "Turns on.";
			} else {
				msg = "Turns off.";
			}
			return 1;
		}
		if ( word2 == "TIME") {
			ogame->game_showtime = !ogame->game_showtime;
			if ( ogame->game_showtime ) {
				msg = "Time on.";
			} else {
				msg = "Time off.";
			}
			return 1;
		}
		if ( word2 == "MUSICCONTROL") {
			game_musiccontrol = !game_musiccontrol;
			msg = "Music Control ";
			if (game_musiccontrol) {
				msg += "on.";
			} else {
				msg += "off.";
			}
		}

		if (word2 == "DEBUGFOCUS") {
			auto focus = oCNpc::player->GetFocusVob();
			if (!focus ) {
				msg =  "No focus Vob.";
				return 1;
			}
			if ( ogame->debugAllInstances ) {
				msg = "Not possible while debugging all instances.";
				return 1;
			}

			int index =  ogame->debugInstances.Search(focus);
			if (index < 0) {
				ogame->debugInstances.Insert(focus);
				msg = "Started debugging Focus object";
				return 1;
			}

			ogame->debugInstances.Remove(focus);
			msg = "Stopped debugging Focus object";
			return 1;
		}

		if (word2 ==  "DEBUGCHANNEL") {
			int index = word3.ToInt();
			if (index < 1 || index > 32) {
				msg = "Channel must be an integral value between 1 and 32.";
				return 1;
			}

			uint32_t bit = 1 << (index - 1);
			ogame->debugChannels ^= bit;
			if ( ogame->debugChannels & bit ) {
				msg = "Channel activated";
			} else {
				msg = "Channel deactivated";
			}
			return 1;
		}

		if (word2 == "ALLDEBUGCHANNELS") {
			ogame->debugChannels = !ogame->debugChannels;
			msg = "All channels toggled";
		}

		msg =  "Unknown Command : " + word2;
		return 0;
	}

	if (word1 == "START") {
		if (word2 != "PARTICLEFX")
			return 0;

		if ( !pfxc )
			pfxc = new oCParticleControl(ogame->GetGameWorld());

		pfxc->SetPFXName(word3);

		auto word4 = str.PickWord(4, " ", zSTR_SKIP);

		if (word4 == "") {
			auto at = oCNpc::player->trafoObjToWorld.GetAtVector();
			auto pos = oCNpc::player->trafoObjToWorld.GetTranslation();

			pfxc->SetStartPos(pos + at * 200.0);
			pfxc->StartActivePFX();
		}

		msg = word3 + " started.";
		return 1;
	}


	if ( word1 == "CREATE") {
		if (word2 != "PARTICLEFX")
			return 0;

		if ( !pfxc )
			pfxc = new oCParticleControl(ogame->GetGameWorld());

		pfxc->CreateNewPFX(word3);

		msg = word3 + " created.";
		return 1;
	}
	if ( word1 == "EDIT") {
		if ( word2 == "PARTICLEFX") {
			if ( !pfxc )
				pfxc = new oCParticleControl(ogame->GetGameWorld());
			
			zcon.Hide();

			if ( pfxc ) {
				pfxc->SetPFXName(word3);
				pfxc->BeginEditActive(Game_Particle_Changed);
			}
			return 1;
		}
		if ( word2 == "COMBAT") {
			zcon.Hide();
			Game_OpenFightConsole();
			return 1;
		}
		if ( word2 == "SPECIES") {
			Game_OpenSpeciesConsole();
			return 1;
		}
		if ( word2 == "AI") {
			Game_ToggleAIConsole();
			return 1;
		}
		if ( word2 == "ABILITIES") {
			zcon.Hide();

			if ( edit_con )
				delete edit_con;

			edit_con = new zCConsole;
			edit_con->SetPos(0, 0);
			edit_con->SetParser(zparser);
			edit_con->EditInstance(oCNpc::player->GetInstance(), oCNpc::player);
			edit_con->SetChangedFunc(Edit_Changed);
			return 1;
		}
		if (word2 == "FOCUS") {
			auto focus = oCNpc::player->GetFocusVob();
			if (!focus) {
				msg = "No focus Vob.";
				return 1;
			}

			zcon.Hide();

			if ( edit_con )
				delete edit_con;

			edit_con = new zCConsole;
			edit_con->SetPos(0, 0);
			edit_con->SetParser(zparser);

			if ( focus->type != VOB_TYPE_ITEM && focus->type != VOB_TYPE_NPC )
				return 1;

			edit_con->EditInstance(focus->GetInstance(), focus);
			return 1;
		}

		if (word2 == "THROW")
			auto focus = oCNpc::player->GetFocusVob();

			zcon.Hide();

			if ( edit_con )
				delete edit_con;

			edit_con = new zCConsole;
			edit_con->SetPos(0, 0);
			edit_con->SetParser(zparser);

			edit_con->EditInstance(oCNpcFocus::GetFocusName(), oCNpcFocus::focus);

			return 1;
		}

		return 0;
	}

