//statics:

class zCSession {
public:
	virtual ~zCSession();
	void CamInit(zCVob* camVob, zCCamera* camera);
private:
	zCCSManager*  csMan;
	zCWorld*      world;
	zCCamera*     camera;
	zCAICamera*   aiCam;
	zCVob*        camVob;
	zCView*       viewport;
};

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

int oCGame::HandleEvent(int key)
{
	double v2; // st7@0
	zCWorld *world; // eax@1 MAPDST
	zSTRING__vtable *v5; // edi@3
	oCNpc *player; // ecx@17
	struct zCEventManager *v8; // eax@20
	oCAniCtrl_Human *v9; // eax@21
	int sym; // esi@24
	int v11; // esi@28
	struct zCEventManager *v12; // eax@35
	oCAniCtrl_Human *v13; // eax@37
	int v14; // esi@41
	char v15; // cl@42
	char *v16; // eax@42
	char v18; // cl@48
	char *v19; // eax@48
	char v20; // cl@55
	char *v21; // eax@55
	char v22; // cl@61
	char *v23; // eax@61
	int v24; // eax@70
	zCWorld *v25; // eax@74
	struct zCSkyControler *v26; // eax@74
	oCMag_Book *v27; // eax@77
	struct oCMobInter *v28; // eax@81
	struct oCMobInter *v29; // eax@82
	struct oCMobInter *v30; // eax@82
	struct zCObject *v31; // esi@84
	zCEventMessage *v32; // esi@86
	struct zCObject *v33; // esi@87
	struct zCEventManager *v34; // eax@90
	struct zCObject *v35; // esi@91
	zCEventMessage *v36; // esi@92
	oCMag_Book *v37; // eax@96
	struct oCMobInter *v38; // eax@99
	struct oCMobInter *v39; // eax@100
	struct oCMobInter *v40; // eax@100
	struct oCItem *v41; // eax@102
	struct zCObject *v42; // esi@104
	struct oCItem *v43; // eax@108
	struct zCObject *v44; // esi@110
	zCEventMessage *v45; // esi@111
	struct zCEventManager *v46; // eax@113
	struct zCObject *v47; // esi@113
	struct oCMobInter *v48; // eax@121
	struct oCMobInter *v49; // eax@122
	struct oCMobInter *v50; // eax@122
	struct oCMobInter *v51; // eax@123
	struct oCMobInter *v52; // eax@124
	struct oCMobInter *v53; // eax@124
	int v54; // eax@125
	struct zCObject *v55; // esi@127
	zCEventMessage *v56; // esi@128
	struct zCEventManager *v57; // eax@130
	struct zCObject *v58; // esi@131
	signed int v59; // esi@135
	struct oCMobInter *v60; // eax@138
	struct oCMobInter *v61; // eax@139
	struct oCMobInter *v62; // eax@139
	oCMag_Book *v63; // eax@142
	oCMag_Book *v64; // eax@143
	oCMag_Book *v65; // eax@150
	int v66; // eax@150
	int v67; // ST1C_4@151
	oCMag_Book *v68; // eax@151
	oCMag_Book *v69; // eax@152
	oCSpell *v70; // eax@152
	oCSpell *v71; // esi@152
	struct zSTRING *v72; // eax@153
	char v73; // cl@154
	void *v74; // eax@154
	struct zSTRING *v75; // eax@158
	char v76; // cl@159
	void *v77; // eax@159
	int v78; // eax@165
	struct zCObject *v79; // esi@167
	zCEventMessage *v80; // esi@168
	struct zCEventManager *v81; // eax@170
	struct zCObject *v82; // esi@171
	struct zCEventManager *v83; // eax@174
	char v84; // cl@179
	double v85; // st7@185
	double v86; // st6@185
	char v87; // cl@187
	char *v88; // eax@187
	char v89; // cl@191
	char *v90; // eax@191
	char v91; // cl@196
	char v92; // [sp+13h] [bp-EDh]@0
	char v93; // [sp+18h] [bp-E8h]@1
	float v94; // [sp+18h] [bp-E8h]@46
	std::string v95; // [sp+20h] [bp-E0h]@3
	struct zSTRING v96; // [sp+30h] [bp-D0h]@54
	struct zSTRING v97; // [sp+44h] [bp-BCh]@60
	struct zSTRING v98; // [sp+58h] [bp-A8h]@47
	float v99; // [sp+6Ch] [bp-94h]@185
	float v100; // [sp+70h] [bp-90h]@185
	float v101; // [sp+74h] [bp-8Ch]@185
	float v102; // [sp+78h] [bp-88h]@70
	struct zSTRING v103; // [sp+7Ch] [bp-84h]@53
	zSTRING__vtable *v104; // [sp+90h] [bp-70h]@153
	int v105; // [sp+98h] [bp-68h]@153
	int v106; // [sp+9Ch] [bp-64h]@158
	int v107; // [sp+A0h] [bp-60h]@158
	zSTRING__vtable *v108; // [sp+A4h] [bp-5Ch]@158
	int v109; // [sp+ACh] [bp-54h]@158
	int v110; // [sp+B0h] [bp-50h]@163
	int v111; // [sp+B4h] [bp-4Ch]@163
	zSTRING msg; // [sp+B8h] [bp-48h]@14
	zSTRING symname; // [sp+CCh] [bp-34h]@24
	zSTRING v114; // [sp+E0h] [bp-20h]@25
	int v115; // [sp+FCh] [bp-4h]@3

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
		zERR_INFO8("N: MAP: GAME_SCREEN_MAP");

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
			int sym = zparser->GetIndex("PLAYER_HOTKEY_SCREEN_MAP");
			if (!sym)
			{
				zERR_FAULT("N: MAP: Skript function PLAYER_HOTKEY_SCREEN_MAP not found!");
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
		if ( flt_99B3D8 != 0.0 && player->IsUnconscious() )
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
		if ( !game_testmode )
		{
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
		if ( player->GetWeaponMode() <= FMODE_MAGIC )
		{
			if ( player->GetWeaponMode() != FMODE_NONE )
			{
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
